#pragma once

#include "../task.h"
#include "../proto/proto_gen/smartknob.pb.h"
#include <Wire.h>

// I2C slave device address (configurable)
#ifndef I2C_P4_SLAVE_ADDRESS
#define I2C_P4_SLAVE_ADDRESS 0x50
#endif

class I2CP4Task : public Task<I2CP4Task>
{
    friend class Task<I2CP4Task>;

public:
    I2CP4Task(const uint8_t task_core, uint8_t slave_address = I2C_P4_SLAVE_ADDRESS);
    ~I2CP4Task();

    // Send the current SmartKnob config to the external controller.
    void sendConfig(const PB_SmartKnobConfig &config);

private:
    void run();

    bool writeToSlave(uint8_t reg, const uint8_t *data, size_t len);
    bool encodeConfigBuffer(const PB_SmartKnobConfig &config, size_t &encoded_size);
    bool flushPendingConfigLocked();

    static constexpr uint8_t REG_PROTOBUF_CONFIG = 0x00;
    static constexpr size_t CONFIG_MESSAGE_PAYLOAD_MAX = PB_SmartKnobConfig_size + 64;
    static constexpr size_t CONFIG_MESSAGE_MAX_SIZE = CONFIG_MESSAGE_PAYLOAD_MAX + sizeof(uint16_t);
    static constexpr uint32_t CONFIG_RESEND_INTERVAL_MS = 100;

    bool has_last_config_;
    bool pending_config_valid_;
    bool pending_force_send_;
    PB_SmartKnobConfig last_sent_config_;
    PB_SmartKnobConfig pending_config_;
    uint8_t config_tx_buffer_[CONFIG_MESSAGE_MAX_SIZE];

    SemaphoreHandle_t mutex_;
    uint8_t slave_address_;
    bool slave_available_;
    unsigned long last_write_time_;
    unsigned long last_periodic_send_ms_;
};
