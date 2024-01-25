#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "../proto_gen/smartknob.pb.h"
#include "../app_config.h"
#include "icons.h"

const char APP_SLUG_CLIMATE[48] = "thermostat";
const char APP_SLUG_BLINDS[48] = "blinds";
const char APP_SLUG_MUSIC[48] = "music";
const char APP_SLUG_3D_PRINTER[48] = "3d_printer";
const char APP_SLUG_LIGHT_DIMMER[48] = "light_dimmer";
const char APP_SLUG_LIGHT_SWITCH[48] = "light_switch";
const char APP_SLUG_STOPWATCH[48] = "stopwatch";

enum AppId : int8_t
{
    MENU = -2,
    DONT_NAVIGATE = -1,
    PRINTER_CHAMBER = 0,
    BLINDS = 1,
    CLIMATE = 2,
    LIGHT_DIMMER = 3,
    LIGHT_SWITCH = 4,
    MUSIC = 5,
    SETTINGS = 6,
    STOPWATCH = 7,
    HASS_SETUP_APP = 8,
    DEMO = INT8_MAX,

};

class App
{
public:
    int8_t next = DONT_NAVIGATE;
    int8_t back = MENU;

    App(TFT_eSprite *spr_) : spr_(spr_) {}
    App(TFT_eSprite *spr_, int8_t next, int8_t back) : spr_(spr_), next(next), back(back) {}
    virtual TFT_eSprite *render();
    virtual EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    virtual void updateStateFromSystem(AppState state);

    PB_SmartKnobConfig getMotorConfig()
    {
        return motor_config;
    }

    std::string getClassName()
    {
        return "App";
    }

    const unsigned char *small_icon;
    const unsigned char *big_icon;
    const char *friendly_name = "";

protected:
    /** Full-size sprite used as a framebuffer */
    TFT_eSprite *spr_;
    PB_SmartKnobConfig motor_config;
};