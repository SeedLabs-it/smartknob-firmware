#include "i2c_p4_task.h"

#include "semaphore_guard.h"
#include "../proto/proto_helpers.h"
#include "../app_config.h"

#include <pb_decode.h>
#include <pb_encode.h>

I2CP4Task::I2CP4Task(const uint8_t task_core, uint8_t slave_address)
    : Task{"I2CP4", 4096, 1, task_core},
      has_last_config_(false),
      pending_config_valid_(false),
      pending_force_send_(false),
      slave_address_(slave_address),
      slave_available_(false),
      last_write_time_(0),
      last_periodic_send_ms_(0),
      last_rx_poll_ms_(0),
      received_config_queue_(nullptr)
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
    received_config_queue_ = xQueueCreate(2, sizeof(PB_SmartKnobConfig));
    assert(received_config_queue_ != NULL);
}

I2CP4Task::~I2CP4Task()
{
    vSemaphoreDelete(mutex_);
    if (received_config_queue_ != nullptr)
    {
        vQueueDelete(received_config_queue_);
    }
}

void I2CP4Task::run()
{
    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);
    Wire.setTimeOut(5);

    // Give other tasks (especially sensors) time to initialize
    // Sensors need ~3-4 seconds to boot VL53L0X and VEML7700
    vTaskDelay(pdMS_TO_TICKS(5000));

#if WIRE_HAS_BUFFER_SIZE
    if (CONFIG_MESSAGE_MAX_SIZE > I2C_BUFFER_LENGTH)
    {
        Wire.setBufferSize(CONFIG_MESSAGE_MAX_SIZE);
    }
#endif

    LOGI("I2C P4 task started - will write to slave device at address 0x%02X", slave_address_);

    Wire.beginTransmission(slave_address_);
    uint8_t error = Wire.endTransmission();

    if (error == 0)
    {
        slave_available_ = true;
        LOGI("I2C slave device detected at 0x%02X", slave_address_);
    }
    else
    {
        LOGW("I2C slave device not found at 0x%02X (error: %d)", slave_address_, error);
        LOGW("Will continue trying to send configs...");
    }

    while (1)
    {
        const unsigned long now = millis();

        // Periodically check if slave is available
        if (now - last_write_time_ > 5000)
        {
            Wire.beginTransmission(slave_address_);
            uint8_t ping_error = Wire.endTransmission();
            slave_available_ = (ping_error == 0);
        }

        PB_SmartKnobConfig config_to_send;
        bool has_config_to_send = false;
        bool should_poll_rx = false;

        {
            SemaphoreGuard lock(mutex_);
            const bool should_queue_periodic =
                !pending_config_valid_ &&
                has_last_config_ &&
                (now - last_periodic_send_ms_ >= CONFIG_RESEND_INTERVAL_MS);
            if (should_queue_periodic)
            {
                pending_config_ = last_sent_config_;
                pending_config_valid_ = true;
                pending_force_send_ = true;
            }
            has_config_to_send = popPendingConfigLocked(config_to_send);

            if (slave_available_ && (now - last_rx_poll_ms_ >= CONFIG_RX_POLL_INTERVAL_MS))
            {
                should_poll_rx = true;
                last_rx_poll_ms_ = now;
            }
        }

        if (has_config_to_send)
        {
            size_t encoded_size = 0;
            if (!encodeConfigBuffer(config_to_send, encoded_size))
            {
                requeueConfigForRetry(config_to_send);
            }
            else if (!writeToSlave(REG_PROTOBUF_CONFIG, config_tx_buffer_, encoded_size))
            {
                LOGE("I2C P4 Failed to send SmartKnob config to I2C slave");
                requeueConfigForRetry(config_to_send);
            }
            else
            {
                SemaphoreGuard lock(mutex_);
                last_sent_config_ = config_to_send;
                has_last_config_ = true;
                pending_force_send_ = false;
                last_periodic_send_ms_ = millis();
            }
        }

        if (should_poll_rx)
        {
            pollIncomingConfig();
        }

        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

void I2CP4Task::sendConfig(const PB_SmartKnobConfig &config)
{
    SemaphoreGuard lock(mutex_);

    pending_config_ = config;
    pending_config_valid_ = true;
    pending_force_send_ = true;
}

bool I2CP4Task::encodeConfigBuffer(const PB_SmartKnobConfig &config, size_t &encoded_size)
{
    size_t payload_size = 0;
    if (!pb_get_encoded_size(&payload_size, PB_SmartKnobConfig_fields, &config))
    {
        LOGE("Failed to measure SmartKnob config size");
        return false;
    }

    if (payload_size > CONFIG_MESSAGE_PAYLOAD_MAX)
    {
        LOGE("SmartKnob config payload too large (%u bytes > %u)", payload_size, CONFIG_MESSAGE_PAYLOAD_MAX);
        return false;
    }

    pb_ostream_t stream = pb_ostream_from_buffer(
        config_tx_buffer_ + sizeof(uint16_t),
        CONFIG_MESSAGE_PAYLOAD_MAX);

    if (!pb_encode(&stream, PB_SmartKnobConfig_fields, &config))
    {
        const char *error = stream.errmsg ? stream.errmsg : "unknown";
        LOGE("Failed to encode SmartKnob config: %s", error);
        return false;
    }

    if (stream.bytes_written > 0xFFFF)
    {
        LOGE("Encoded SmartKnob config too large: %u bytes", stream.bytes_written);
        return false;
    }

    const uint16_t encoded_payload_size = static_cast<uint16_t>(stream.bytes_written);
    config_tx_buffer_[0] = encoded_payload_size & 0xFF;
    config_tx_buffer_[1] = (encoded_payload_size >> 8) & 0xFF;
    encoded_size = encoded_payload_size + sizeof(uint16_t);
    return true;
}

bool I2CP4Task::getNextReceivedConfig(PB_SmartKnobConfig *config)
{
    (void)config;
    return false;
}

bool I2CP4Task::popPendingConfigLocked(PB_SmartKnobConfig &config)
{
    if (!pending_config_valid_)
    {
        return false;
    }

    const bool config_matches_last = has_last_config_ && config_eq(last_sent_config_, pending_config_);
    if (config_matches_last && slave_available_ && !pending_force_send_)
    {
        pending_config_valid_ = false;
        return false;
    }

    config = pending_config_;
    pending_config_valid_ = false;
    pending_force_send_ = false;
    return true;
}

void I2CP4Task::requeueConfigForRetry(const PB_SmartKnobConfig &config)
{
    SemaphoreGuard lock(mutex_);
    pending_config_ = config;
    pending_config_valid_ = true;
    pending_force_send_ = true;
}

bool I2CP4Task::writeToSlave(uint8_t reg, const uint8_t *data, size_t len)
{
    Wire.beginTransmission(slave_address_);
    Wire.write(reg);
    Wire.write(data, len);
    uint8_t error = Wire.endTransmission();

    last_write_time_ = millis();

    if (error != 0)
    {
        LOGD("I2C write failed: error=%d, reg=0x%02X", error, reg);
        slave_available_ = false;
        return false;
    }

    if (!slave_available_)
    {
        slave_available_ = true;
        LOGI("I2C slave device reconnected");
    }

    return true;
}

void I2CP4Task::pollIncomingConfig()
{
    if (!slave_available_)
    {
        return;
    }

    size_t bytes_read = 0;
    if (!readConfigMessage(bytes_read))
    {
        return;
    }

    if (bytes_read < CONFIG_RX_MESSAGE_OVERHEAD)
    {
        LOGI("I2C P4 Received I2C payload too small (%u bytes) from P4", static_cast<unsigned int>(bytes_read));
        return;
    }

    const uint8_t reg = config_rx_buffer_[0];
    const uint16_t declared_payload_len = config_rx_buffer_[1] | (config_rx_buffer_[2] << 8);
    const size_t payload_len = bytes_read - CONFIG_RX_MESSAGE_OVERHEAD;

    if (reg != REG_PROTOBUF_CONFIG)
    {
        LOGI("I2C P4 Received message from P4 on register 0x%02X (len=%u)", reg, static_cast<unsigned int>(payload_len));
        return;
    }

    if (payload_len == 0)
    {
        LOGI("I2C P4 Received empty SmartKnob config payload from P4");
        return;
    }

    if (declared_payload_len > payload_len)
    {
        LOGI("I2C P4 Truncated SmartKnob payload from P4 (declared=%u actual=%u)",
             declared_payload_len, static_cast<unsigned int>(payload_len));
        return;
    }

    PB_SmartKnobConfig config = PB_SmartKnobConfig_init_default;
    pb_istream_t stream = pb_istream_from_buffer(config_rx_buffer_ + CONFIG_RX_MESSAGE_OVERHEAD, declared_payload_len);
    if (!pb_decode(&stream, PB_SmartKnobConfig_fields, &config))
    {
        const char *error = stream.errmsg ? stream.errmsg : "unknown";
        LOGE("I2C P4 Failed to decode SmartKnob config from P4: %s", error);
        return;
    }

    LOGI("I2C P4 received smartknob config id '%s' from P4", config.id);
}

bool I2CP4Task::readConfigMessage(size_t &bytes_read)
{
    Wire.beginTransmission(slave_address_);
    Wire.write(REG_PROTOBUF_CONFIG);
    uint8_t error = Wire.endTransmission(false);
    if (error != 0)
    {
        LOGD("I2C P4 Failed to request config register: err=%d", error);
        slave_available_ = false;
        return false;
    }

    bytes_read = Wire.requestFrom(static_cast<uint8_t>(slave_address_), CONFIG_MESSAGE_MAX_SIZE, true);
    if (bytes_read == 0)
    {
        return false;
    }
    if (bytes_read > CONFIG_MESSAGE_MAX_SIZE)
    {
        bytes_read = CONFIG_MESSAGE_MAX_SIZE;
    }

    size_t copied = 0;
    while (Wire.available() > 0 && copied < bytes_read)
    {
        config_rx_buffer_[copied++] = Wire.read();
    }
    while (Wire.available() > 0)
    {
        (void)Wire.read();
    }

    bytes_read = copied;
    return bytes_read >= CONFIG_RX_MESSAGE_OVERHEAD;
}
