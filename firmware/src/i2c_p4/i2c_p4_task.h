#pragma once

#include "../task.h"
#include "../app_config.h"
#include <Wire.h>

// I2C slave device address (configurable)
#ifndef I2C_P4_SLAVE_ADDRESS
#define I2C_P4_SLAVE_ADDRESS 0x50
#endif

// Register addresses on the slave device
#define REG_MOTOR_POSITION 0x00     // int32_t (4 bytes)
#define REG_MOTOR_SUB_POSITION 0x04 // float (4 bytes)
#define REG_BRIGHTNESS 0x08         // int32_t (4 bytes)
#define REG_HUE 0x0C                // int32_t (4 bytes)
#define REG_TEMPERATURE 0x10        // int32_t (4 bytes)
#define REG_APP_ID 0x14             // string (64 bytes)
#define REG_ENTITY_ID 0x54          // string (64 bytes)
#define REG_STATE_JSON 0x94         // string (96 bytes)

class I2CP4Task : public Task<I2CP4Task>
{
    friend class Task<I2CP4Task>;

public:
    I2CP4Task(const uint8_t task_core, uint8_t slave_address = I2C_P4_SLAVE_ADDRESS);
    ~I2CP4Task();

    // Update state from RootTask/Apps (writes to I2C slave device)
    void updateMotorState(const PB_SmartKnobState &state);
    void updateEntityState(const EntityStateUpdate &state);
    void updateAppPositions(int32_t brightness, int32_t hue, int32_t temp);

private:
    void run();

    bool writeToSlave(uint8_t reg, const uint8_t *data, size_t len);
    bool writeInt32ToSlave(uint8_t reg, int32_t value);
    bool writeFloatToSlave(uint8_t reg, float value);
    bool writeStringToSlave(uint8_t reg, const char *str, size_t max_len);

    // State cache (only write when changed)
    int32_t last_motor_position_;
    float last_motor_sub_position_;
    int32_t last_brightness_;
    int32_t last_hue_;
    int32_t last_temperature_;
    char last_app_id_[64];
    char last_entity_id_[64];

    SemaphoreHandle_t mutex_;
    uint8_t slave_address_;
    bool slave_available_;
    unsigned long last_write_time_;
};
