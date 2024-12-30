#include "motor_testing.h"
#include "cJSON.h"
#include <cstring>

MotorTestingApp::MotorTestingApp(PB_SmartKnobConfig motor_config__) : App(xSemaphoreCreateMutex())
{
    snprintf(app_id, sizeof(app_id) - 1, "%s", motor_config__.id);
    motor_config = motor_config__;
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "MOTOR_TESTING");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    label_pos = lv_label_create(screen);
    lv_label_set_text(label_pos, "0");
    lv_obj_align(label_pos, LV_ALIGN_CENTER, 0, 30);
}

void MotorTestingApp::handleNavigation(NavigationEvent event)
{
}

int8_t MotorTestingApp::navigationNext()
{
    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}

int8_t MotorTestingApp::navigationBack()
{
    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}

EntityStateUpdate MotorTestingApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;

    if (state_sent_from_hass)
    {
        state_sent_from_hass = false;
        return new_state;
    }

    current_position = state.current_position;

    sub_position_unit = state.sub_position_unit;
    // //! needed to next reload of App
    // motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    adjusted_sub_position = sub_position_unit * motor_config.position_width_radians;

    if (state.current_position == motor_config.min_position && sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    if (last_position != current_position && !first_run)
    {
        SemaphoreGuard lock(mutex_);
        lv_label_set_text_fmt(label_pos, "%d", current_position);
    }

    last_position = current_position;
    first_run = false;
    return new_state;
}