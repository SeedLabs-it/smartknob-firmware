#pragma once

#include "app.h"

class DemoMotorConfigApp : public App
{
public:
    explicit DemoMotorConfigApp(SemaphoreHandle_t mutex);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) override;
    void updateStateFromSystem(AppState state) override;

private:
    PB_SmartKnobConfig demo_motor_config = PB_SmartKnobConfig{
        .position = 0,
        .sub_position_unit = 0,
        .position_nonce = 0,
        .min_position = 0,
        .max_position = 100,
        .position_width_radians = 2.4 * PI / 180,
        .detent_strength_unit = 1,
        .endstop_strength_unit = 1,
        .snap_point = 1.1,
        .detent_positions_count = 0,
        .detent_positions = {},
        .snap_point_bias = 0,
        .led_hue = 27,
    };

    void initScreen() override;
    void refreshConfigLabel(const PB_SmartKnobState &state);

    lv_obj_t *title_label = nullptr;
    lv_obj_t *config_label = nullptr;
    char config_text_[256];
};
