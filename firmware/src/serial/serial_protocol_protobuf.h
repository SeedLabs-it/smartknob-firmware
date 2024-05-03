#pragma once

#include <PacketSerial.h>

#include "../proto_gen/smartknob.pb.h"

#include "interface_callbacks.h"
#include "motor_foc/motor_task.h"
#include "serial_protocol.h"
#include "uart_stream.h"

class SerialProtocolProtobuf : public SerialProtocol
{
public:
    SerialProtocolProtobuf(Stream &stream, Configuration *configuration, ConfigCallback config_callback, MotorCalibrationCallback motor_calibration_callback, FactoryStrainCalibrationCallback factory_strain_calibration_callback);
    ~SerialProtocolProtobuf(){};
    void log(const char *msg) override;
    void log(const PB_LogLevel log_level, bool isVerbose_, const char *origin, const char *msg) override;
    void sendInitialInfo();
    void sendStrainCalibState(const uint8_t step);
    void loop() override;
    void handleState(const PB_SmartKnobState &state) override;

private:
    Stream &stream_;
    Configuration *configuration_;
    ConfigCallback config_callback_;
    MotorCalibrationCallback motor_calibration_callback_;
    StrainCalibrationCallback strain_calibration_callback_;

    PB_FromSmartKnob pb_tx_buffer_;
    PB_ToSmartknob pb_rx_buffer_;

    uint8_t tx_buffer_[PB_FromSmartKnob_size + 4]; // Max message size + CRC32

    PacketSerial_<COBS, 0, (PB_ToSmartknob_size + 4) * 2 + 10> packet_serial_;

    uint32_t last_nonce_;

    PB_SmartKnobState latest_state_ = {};
    PB_SmartKnobState last_sent_state_ = {};
    uint32_t last_sent_state_millis_ = 0;

    bool state_requested_;

    void sendPbTxBuffer();
    void handlePacket(const uint8_t *buffer, size_t size);
    void ack(uint32_t nonce);
};
