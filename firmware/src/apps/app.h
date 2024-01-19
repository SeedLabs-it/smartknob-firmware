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

enum app_types
{
    menu_type = 1,
    apps_type = 2
};

typedef uint8_t id;
class App
{

public:
    // ID for app

    const app_types type = apps_type;

    const unsigned char *small_icon;
    const unsigned char *big_icon;
    const char *friendly_name;

    App(TFT_eSprite *spr_)
    {
        this->spr_ = spr_;
    }
    virtual ~App() {}

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

    virtual std::pair<app_types, uint8_t> navigationNext()
    {
        if (type == menu_type)
            return std::make_pair(apps_type, 1);

        return std::make_pair(menu_type, 0);
    }

    virtual TFT_eSprite *render();

protected:
    /** Full-size sprite used as a framebuffer */
    TFT_eSprite *spr_;
    PB_SmartKnobConfig motor_config;
};