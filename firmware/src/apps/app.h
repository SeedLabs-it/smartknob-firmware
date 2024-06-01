#pragma once

#include <Arduino.h>
#include <lvgl.h>

#include "semaphore_guard.h"
#include "../proto_gen/smartknob.pb.h"
#include "../app_config.h"
#include "icons.h"
#include "../events/events.h"
#include "../notify/motor_notifier/motor_notifier.h"

const char APP_SLUG_CLIMATE[48] = "climate";
const char APP_SLUG_BLINDS[48] = "blinds";
const char APP_SLUG_MUSIC[48] = "music";
const char APP_SLUG_3D_PRINTER[48] = "3d_printer";
const char APP_SLUG_LIGHT_DIMMER[48] = "light_dimmer";
const char APP_SLUG_LIGHT_SWITCH[48] = "light_switch";
const char APP_SLUG_STOPWATCH[48] = "stopwatch";

enum SharedAppIds : int8_t
{
    MENU = -2,
    DONT_NAVIGATE = -1,
    DONT_NAVIGATE_UPDATE_MOTOR_CONFIG = -3,
};

class App
{
public:
    App(SemaphoreHandle_t mutex) : mutex_(mutex)
    {
        lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
        lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    }

    App(SemaphoreHandle_t mutex, int8_t next, int8_t back) : mutex_(mutex), next_(next), back_(back)
    {
        lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
        lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
    }

    void render()
    {
        SemaphoreGuard lock(mutex_);
        lv_scr_load(screen);
    }

    virtual EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) { return EntityStateUpdate(); };
    virtual void updateStateFromHASS(MQTTStateUpdate mqtt_state_update) {};
    virtual void updateStateFromSystem(AppState state) {};

    void setMotorNotifier(MotorNotifier *motor_notifier)
    {
        this->motor_notifier = motor_notifier;
    }

    void triggerMotorConfigUpdate()
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

    virtual int8_t navigationNext()
    {
        return next_;
    }
    void setNext(int8_t next)
    {
        next_ = next;
    }
    virtual int8_t navigationBack()
    {
        return back_;
    }
    void setBack(int8_t back)
    {
        back_ = back;
    }

    PB_SmartKnobConfig getMotorConfig()
    {
        return motor_config;
    }

    std::string getClassName()
    {
        return "App";
    }

    lv_img_dsc_t small_icon;
    lv_img_dsc_t big_icon;
    char friendly_name[64] = "";
    char app_id[64] = "";
    char entity_id[64] = "";

protected:
    virtual void initScreen() {};

    /** Full-size sprite used as a framebuffer */
    SemaphoreHandle_t mutex_;
    int8_t next_ = DONT_NAVIGATE;
    int8_t back_ = MENU;
    PB_SmartKnobConfig motor_config;

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config;

    bool state_sent_from_hass = false;

    MotorNotifier *motor_notifier;

    // LVGL
    lv_obj_t *screen = lv_obj_create(NULL);
};