#include "i2c_p4_task.h"

#include "semaphore_guard.h"
#include "../proto/proto_helpers.h"

#include <pb_encode.h>

I2CP4Task::I2CP4Task(const uint8_t task_core, uint8_t slave_address)
    : Task{"I2CP4", 4096, 1, task_core},
      has_last_config_(false),
      pending_config_valid_(false),
      pending_force_send_(false),
      slave_address_(slave_address),
      slave_available_(false),
      last_write_time_(0),
      last_periodic_send_ms_(0)
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
}

I2CP4Task::~I2CP4Task()
{
    vSemaphoreDelete(mutex_);
}

void I2CP4Task::run()
{
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

            if (pending_config_valid_)
            {
                flushPendingConfigLocked();
            }
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
    if (!flushPendingConfigLocked())
    {
        LOGW("Queued SmartKnob config for retry over I2C");
    }
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

bool I2CP4Task::flushPendingConfigLocked()
{
    if (!pending_config_valid_)
    {
        return true;
    }

    const bool config_matches_last = has_last_config_ && config_eq(last_sent_config_, pending_config_);
    if (config_matches_last && slave_available_ && !pending_force_send_)
    {
        pending_config_valid_ = false;
        return true;
    }

    size_t encoded_size = 0;
    if (!encodeConfigBuffer(pending_config_, encoded_size))
    {
        pending_config_valid_ = false;
        return false;
    }

    if (!writeToSlave(REG_PROTOBUF_CONFIG, config_tx_buffer_, encoded_size))
    {
        return false;
    }

    last_sent_config_ = pending_config_;
    has_last_config_ = true;
    pending_config_valid_ = false;
    pending_force_send_ = false;
    last_periodic_send_ms_ = millis();
    return true;
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
