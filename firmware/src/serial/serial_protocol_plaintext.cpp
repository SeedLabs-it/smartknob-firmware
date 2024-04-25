#include "../proto_gen/smartknob.pb.h"

#include "serial_protocol_plaintext.h"

void SerialProtocolPlaintext::handleState(const PB_SmartKnobState &state)
{
    bool substantial_change = (latest_state_.current_position != state.current_position) || (latest_state_.config.detent_strength_unit != state.config.detent_strength_unit) || (latest_state_.config.endstop_strength_unit != state.config.endstop_strength_unit) || (latest_state_.config.min_position != state.config.min_position) || (latest_state_.config.max_position != state.config.max_position);
    latest_state_ = state;

    if (substantial_change)
    {

        LOGD("STATE: %d [%d, %d]  (detent strength: %0.2f, width: %0.0f deg, endstop strength: %0.2f)",
             state.current_position,
             state.config.min_position,
             state.config.max_position,
             state.config.detent_strength_unit,
             degrees(state.config.position_width_radians),
             state.config.endstop_strength_unit);
    }
}

void SerialProtocolPlaintext::log(const char *msg)
{
    char origin_[256];
    snprintf(origin_, sizeof(origin_), "[%s:%s:%d] ", __FILE__, __func__, __LINE__);
    log(PB_LogLevel_INFO, false, origin_, msg);
}

void SerialProtocolPlaintext::log(const PB_LogLevel log_level, bool isVerbose_, const char *origin, const char *msg)
{

    if (isVerbose_ && !isVerbose())
    {
        return;
    }

    if (logOrigin())
    {
        stream_.printf("[%s]", origin);
    }

    switch (log_level)
    {
    case PB_LogLevel_INFO:
        stream_.print(" INFO: ");
        break;
        break;
    case PB_LogLevel_WARNING:
        stream_.print(" WARNING: ");
        break;
    case PB_LogLevel_ERROR:
        stream_.print(" ERROR: ");
        break;
    case PB_LogLevel_DEBUG:
        stream_.print(" DEBUG: ");
        break;
    default:
        stream_.print(" INFO: ");
        break;
    }
    stream_.println(msg);
}

void SerialProtocolPlaintext::loop()
{
    while (stream_.available() > 0)
    {
        int b = stream_.read();
        if (b == 0 || b == 'q')
        {
            if (protocol_change_callback_)
            {
                protocol_change_callback_(SERIAL_PROTOCOL_PROTO);
            }
            break;
        }
        else if (b == 'Y' || b == 'y')
        {
            factory_strain_calibration_callback_();
        }
        else if (b == 'C' || b == 'c')
        {
            motor_calibration_callback_();
        }
        else if (b == 'S' || b == 's')
        {
            stream_.println("Strain calibration request received\n");

            if (strain_calibration_callback_)
            {
                strain_calibration_callback_();
            }
        }
        else if (b == 'V' || b == 'v')
        {
            stream_.println("Verbose toggle request received\n");
            toggleVerbose();
        }
        else if (b == 'O' || b == 'o')
        {
            stream_.println("Log origin toggle request received\n");
            toggleLogOrigin();
        }
        else if (b == 'M' || b == 'm')
        {
            stream_.println("Change mode request received\n");

            if (operation_mode_toggle_callback_)
            {
                operation_mode_toggle_callback_();
            }
        }
    }
}

void SerialProtocolPlaintext::init(DemoConfigChangeCallback demo_config_change_callback, StrainCalibrationCallback strain_calibration_callback, OperationModeToggleCallback operation_mode_toggle_callback)
{
    demo_config_change_callback_ = demo_config_change_callback;
    strain_calibration_callback_ = strain_calibration_callback;
    operation_mode_toggle_callback_ = operation_mode_toggle_callback;
    stream_.println("SmartKnob starting!\n\nSerial mode: plaintext\nPress 'C' at any time to calibrate motor/sensor.\nPress 'S' at any time to calibrate strain sensors.\nPress <Space> to change haptic modes.\nPress V to stoggle verbose mode.\nPress M to switch from onboarding to real apps and back.");
}
