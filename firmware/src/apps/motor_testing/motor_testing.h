#pragma once
#include "../app.h"
#include "../../util.h"

#define skip_degrees_selectable 4 // TODO should only be in hue.h

class MotorTestingApp : public App
{
public:
    MotorTestingApp(PB_SmartKnobConfig motor_config__);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) override;

    void setMotorConfig(PB_SmartKnobConfig motor_config__)
    {
        motor_config = motor_config__;
    }

    int8_t navigationNext() override;
    int8_t navigationBack() override;

    void handleNavigation(NavigationEvent event) override;

private:
    int16_t current_position = 0;
    int16_t last_position = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;

    bool first_run = true;

    lv_obj_t *label_pos = nullptr;
};