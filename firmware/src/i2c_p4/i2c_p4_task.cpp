#include "i2c_p4_task.h"
#include "semaphore_guard.h"
#include <string.h>

I2CP4Task::I2CP4Task(const uint8_t task_core, uint8_t slave_address)
    : Task{"I2CP4", 4096, 1, task_core},
      slave_address_(slave_address),
      slave_available_(false),
      last_write_time_(0),
      last_motor_position_(0),
      last_motor_sub_position_(0.0f),
      last_brightness_(0),
      last_hue_(0),
      last_temperature_(0)
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    memset(last_app_id_, 0, sizeof(last_app_id_));
    memset(last_entity_id_, 0, sizeof(last_entity_id_));
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

    LOGI("I2C P4 task started - will write to slave device at address 0x%02X", slave_address_);

    // Test if slave device is available
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
        LOGW("Will continue trying to write state updates...");
    }

    // Main loop - just yield, actual writes happen in update methods
    while (1)
    {
        // Periodically check if slave is available
        if (millis() - last_write_time_ > 5000)
        {
            Wire.beginTransmission(slave_address_);
            error = Wire.endTransmission();
            slave_available_ = (error == 0);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void I2CP4Task::updateMotorState(const PB_SmartKnobState &state)
{
    SemaphoreGuard lock(mutex_);

    // Rate limiting: only update every 5ms (200Hz)
    const unsigned long UPDATE_INTERVAL_MS = 5;
    unsigned long now = millis();

    if (now - last_write_time_ < UPDATE_INTERVAL_MS)
    {
        return; // Skip update if too soon
    }

    // Only write if changed
    if (state.current_position != last_motor_position_ ||
        state.sub_position_unit != last_motor_sub_position_)
    {
        if (writeInt32ToSlave(REG_MOTOR_POSITION, state.current_position))
        {
            last_motor_position_ = state.current_position;
        }

        if (writeFloatToSlave(REG_MOTOR_SUB_POSITION, state.sub_position_unit))
        {
            last_motor_sub_position_ = state.sub_position_unit;
        }

        last_write_time_ = now;
    }
}

void I2CP4Task::updateEntityState(const EntityStateUpdate &state)
{
    SemaphoreGuard lock(mutex_);

    if (!state.changed)
        return;

    // Write app ID if changed
    if (strcmp(state.app_id, last_app_id_) != 0)
    {
        if (writeStringToSlave(REG_APP_ID, state.app_id, 64))
        {
            strncpy(last_app_id_, state.app_id, sizeof(last_app_id_) - 1);
        }
    }

    // Write entity ID if changed
    if (strcmp(state.entity_id, last_entity_id_) != 0)
    {
        if (writeStringToSlave(REG_ENTITY_ID, state.entity_id, 64))
        {
            strncpy(last_entity_id_, state.entity_id, sizeof(last_entity_id_) - 1);
        }
    }

    // Write state JSON
    writeStringToSlave(REG_STATE_JSON, state.state, 96);
}

void I2CP4Task::updateAppPositions(int32_t brightness, int32_t hue, int32_t temp)
{
    SemaphoreGuard lock(mutex_);

    if (brightness != last_brightness_)
    {
        if (writeInt32ToSlave(REG_BRIGHTNESS, brightness))
        {
            last_brightness_ = brightness;
        }
    }

    if (hue != last_hue_)
    {
        if (writeInt32ToSlave(REG_HUE, hue))
        {
            last_hue_ = hue;
        }
    }

    if (temp != last_temperature_)
    {
        if (writeInt32ToSlave(REG_TEMPERATURE, temp))
        {
            last_temperature_ = temp;
        }
    }
}

// Helper methods
bool I2CP4Task::writeToSlave(uint8_t reg, const uint8_t *data, size_t len)
{
    Wire.beginTransmission(slave_address_);
    Wire.write(reg);       // Register address
    Wire.write(data, len); // Data
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

bool I2CP4Task::writeInt32ToSlave(uint8_t reg, int32_t value)
{
    uint8_t data[4];
    data[0] = (value >> 0) & 0xFF;
    data[1] = (value >> 8) & 0xFF;
    data[2] = (value >> 16) & 0xFF;
    data[3] = (value >> 24) & 0xFF;

    return writeToSlave(reg, data, 4);
}

bool I2CP4Task::writeFloatToSlave(uint8_t reg, float value)
{
    uint32_t *p = (uint32_t *)&value;
    return writeInt32ToSlave(reg, *p);
}

bool I2CP4Task::writeStringToSlave(uint8_t reg, const char *str, size_t max_len)
{
    size_t len = strlen(str);
    if (len > max_len)
        len = max_len;

    // Write length-prefixed string
    uint8_t data[max_len + 1];
    data[0] = (uint8_t)len;
    memcpy(&data[1], str, len);

    return writeToSlave(reg, data, len + 1);
}
