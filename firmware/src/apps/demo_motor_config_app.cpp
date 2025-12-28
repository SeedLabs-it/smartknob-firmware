#include "demo_motor_config_app.h"

#include <cstdio>

DemoMotorConfigApp::DemoMotorConfigApp(SemaphoreHandle_t mutex) : App(mutex)
{
    LV_IMG_DECLARE(x80_settings);
    LV_IMG_DECLARE(x40_settings);

    snprintf(app_id, sizeof(app_id), "%s", "demo.motor_config");
    snprintf(friendly_name, sizeof(friendly_name), "%s", "Motor Config");

    big_icon = x80_settings;
    small_icon = x40_settings;

    motor_config = demo_motor_config;

    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
    root_level_motor_config = motor_config;

    initScreen();
}

EntityStateUpdate DemoMotorConfigApp::updateStateFromKnob(PB_SmartKnobState state)
{
    refreshConfigLabel(state);
    return EntityStateUpdate();
}

void DemoMotorConfigApp::updateStateFromSystem(AppState state)
{
    refreshConfigLabel(state.motor_state);
}

void DemoMotorConfigApp::initScreen()
{
    SemaphoreGuard lock(mutex_);

    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);

    title_label = lv_label_create(screen);
    lv_label_set_text(title_label, friendly_name);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 24);

    config_label = lv_label_create(screen);
    lv_obj_set_width(config_label, LV_HOR_RES - 32);
    lv_label_set_long_mode(config_label, LV_LABEL_LONG_WRAP);
    lv_label_set_text_static(config_label, "Waiting for motor config...");
    lv_obj_align(config_label, LV_ALIGN_TOP_LEFT, 32, 60);
}

void DemoMotorConfigApp::refreshConfigLabel(const PB_SmartKnobState &state)
{
    if (config_label == nullptr)
    {
        return;
    }

    SemaphoreGuard lock(mutex_);

    if (!state.has_config)
    {
        lv_label_set_text_static(config_label, "Waiting for motor config...");
        return;
    }

    const PB_SmartKnobConfig &config = state.config;

    snprintf(config_text_, sizeof(config_text_),
             "ID: %s\n"
             "Current: %ld (sub %.2f)\n"
             "Bounds: %ld .. %ld\n"
             "Width: %.2f rad\n"
             "Detent/Endstop: %.2f / %.2f\n"
             "Snap: %.2f (bias %.2f)\n",
             config.id,
             state.current_position,
             state.sub_position_unit,
             config.min_position,
             config.max_position,
             config.position_width_radians,
             config.detent_strength_unit,
             config.endstop_strength_unit,
             config.snap_point,
             config.snap_point_bias);

    lv_label_set_text_static(config_label, config_text_);
}
