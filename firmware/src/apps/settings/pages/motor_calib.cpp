#include "motor_calib.h"
#include "root_task.h"

void motor_calib_timer(lv_timer_t *timer)
{
    MotorCalibState *state = (MotorCalibState *)timer->user_data;
    unsigned long now = millis();

    lv_label_set_text_fmt(state->prompt_label, "DONT TOUCH KNOB");
    lv_obj_set_style_text_color(state->prompt_label, LV_COLOR_MAKE(0xFF, 0xB4, 0x50), LV_PART_MAIN);
    lv_label_set_text_fmt(state->time_label, "%02ds", (45000 - (now - state->start_ms)) / 1000);
    if (!state->timer_running)
    {
        lv_obj_align_to(state->time_label, state->prompt_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    }

    if (!state->is_calibrating && (now - state->start_ms) > 5000)
    {
        delete_me_TriggerMotorCalibration();
        state->is_calibrating = true;
    }

    if ((now - state->start_ms) > 45000)
    {
        lv_timer_del(timer);

        lv_label_set_text(state->prompt_label, "PRESS TO START");
        lv_obj_set_style_text_color(state->prompt_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);

        lv_label_set_text(state->time_label, "");

        state->timer_running = false;
        state->is_calibrating = false;
    }

    state->timer_running = true;
}

MotorCalibrationSettingsPage::MotorCalibrationSettingsPage(lv_obj_t *parent) : BasePage(parent)
{
    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Motor Calibration");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -16);

    state_.prompt_label = lv_label_create(page);
    lv_obj_t *prompt_label = state_.prompt_label;
    lv_label_set_text(prompt_label, "PRESS TO START");
    lv_obj_set_style_text_color(prompt_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
    lv_obj_align_to(prompt_label, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);

    state_.time_label = lv_label_create(page);
    lv_obj_t *time_label = state_.time_label;
    lv_label_set_text(time_label, "");
    lv_obj_set_style_text_font(time_label, &roboto_thin_mono_64pt, LV_PART_MAIN);
    lv_obj_align_to(time_label, prompt_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    lv_obj_set_style_text_color(time_label, LV_COLOR_MAKE(0xFF, 0xB4, 0x50), LV_PART_MAIN);
}

void MotorCalibrationSettingsPage::handleNavigation(NavigationEvent event)
{
    if (event == NavigationEvent::SHORT)
    {
        state_.start_ms = millis();
        timer = lv_timer_create(motor_calib_timer, 25, &state_);
    }
}
