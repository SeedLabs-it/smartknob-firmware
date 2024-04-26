#pragma once

#include "../proto_gen/smartknob.pb.h"

#include "interface_callbacks.h"
#include "motor_foc/motor_task.h"
#include "serial_protocol.h"
#include "uart_stream.h"

typedef std::function<void(void)> DemoConfigChangeCallback;
typedef std::function<void(void)> StrainCalibrationCallback;
typedef std::function<void(void)> OperationModeToggleCallback;

class SerialProtocolPlaintext : public SerialProtocol
{
public:
    SerialProtocolPlaintext(Stream &stream, MotorCalibrationCallback motor_calibration_callback, StrainCalibrationCallback strain_calibration_callback, FactoryStrainCalibrationCallback factory_strain_calibration_callback, WeightMeasurementCallback weight_measurement_callback) : SerialProtocol(), stream_(stream), motor_calibration_callback_(motor_calibration_callback), strain_calibration_callback_(strain_calibration_callback), factory_strain_calibration_callback_(factory_strain_calibration_callback), weight_measurement_callback_(weight_measurement_callback) {}
    ~SerialProtocolPlaintext() {}
    void log(const char *msg) override;
    void log(const PB_LogLevel log_level, bool isVerbos, const char *origin, const char *msg) override;
    void loop() override;
    void handleState(const PB_SmartKnobState &state) override;

    void init(DemoConfigChangeCallback demo_config_change_callback, OperationModeToggleCallback operation_mode_toggle_callback);

private:
    Stream &stream_;
    MotorCalibrationCallback motor_calibration_callback_;
    PB_SmartKnobState latest_state_ = {};
    DemoConfigChangeCallback demo_config_change_callback_;
    FactoryStrainCalibrationCallback factory_strain_calibration_callback_;
    WeightMeasurementCallback weight_measurement_callback_;
    StrainCalibrationCallback strain_calibration_callback_;
    OperationModeToggleCallback operation_mode_toggle_callback_;
};
