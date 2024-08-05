#pragma once
#include "./display/page_manager.h"

struct MotorCalibState
{
    unsigned long start_ms;

    bool is_calibrating = false;
    bool timer_running = false;

    lv_obj_t *prompt_label;
    lv_obj_t *time_label;
};

class MotorCalibrationSettingsPage : public BasePage
{
public:
    MotorCalibrationSettingsPage(lv_obj_t *parent);

    void handleNavigation(NavigationEvent event) override;

private:
    MotorCalibState state_;
    lv_timer_t *timer;
};