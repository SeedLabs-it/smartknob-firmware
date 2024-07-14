#include "app.h"

App::App(SemaphoreHandle_t mutex) : mutex_(mutex)
{
    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
}

App::App(SemaphoreHandle_t mutex, int8_t next, int8_t back) : mutex_(mutex), next_(next), back_(back)
{
    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
}

void App::render()
{
    SemaphoreGuard lock(mutex_);
    lv_scr_load(screen);
}

void App::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
}

void App::triggerMotorConfigUpdate()
{
    if (this->motor_notifier != nullptr)
    {
        motor_notifier->requestUpdate(root_level_motor_config);
    }
    else
    {
        LOGW("motor_notifier is not set");
    }
}

void App::setNext(int8_t next)
{
    next_ = next;
}
void App::setBack(int8_t back)
{
    back_ = back;
}

PB_SmartKnobConfig App::getMotorConfig()
{
    return motor_config;
}

std::string App::getClassName()
{
    return "App";
}