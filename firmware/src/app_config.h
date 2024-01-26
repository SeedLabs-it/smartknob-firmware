#pragma once

#include <Arduino.h>
#include "configuration.h"
#include "cJSON.h"

// TODO: move it into the app.h
const uint32_t APP_ID_SETTINGS = 7;
const uint32_t APP_ID_HOME_ASSISTANT = 6;

const uint8_t VIRTUAL_BUTTON_IDLE = 0;
const uint8_t VIRTUAL_BUTTON_SHORT_PRESSED = 1;
const uint8_t VIRTUAL_BUTTON_SHORT_RELEASED = 2;
const uint8_t VIRTUAL_BUTTON_LONG_PRESSED = 3;
const uint8_t VIRTUAL_BUTTON_LONG_RELEASED = 4;

struct ConnectivityState
{
    bool is_connected;
    int8_t signal_strength;
    /* *
  0 - Excellent
  1 - Good
  2 - Fair
  3 - Poor
  4 - No signal
    */
    uint8_t signal_strenth_status;
    std::string ssid;
    std::string ip_address;
};

struct IlluminationState
{
    float lux;
    float lux_avg;
    float lux_adj;
};

// TODO: rename fields to follow the convention
struct ProximityState
{
    uint16_t RangeMilliMeter;
    uint8_t RangeStatus;
};

struct StrainState
{
    int32_t raw_value;
    float press_value;
    uint8_t virtual_button_code;
};

struct SensorsState
{
    ProximityState proximity;
    StrainState strain;
    IlluminationState illumination;
};

struct ScreenState
{
    bool has_been_engaged;
    unsigned long awake_until;
    // where 255 is max and 0 is no light.
    uint16_t MIN_LCD_BRIGHTNESS = UINT16_MAX / 8;
    uint16_t MAX_LCD_BRIGHTNESS = UINT16_MAX;
    uint16_t brightness = MAX_LCD_BRIGHTNESS;
    float luminosityAdjustment = 1;
};

struct AppState
{
    PB_SmartKnobState motor_state;
    ConnectivityState connectivity_state;
    ProximityState proximiti_state;
    ScreenState screen_state;
    cJSON *apps;
};

struct EntityStateUpdate
{
    std::string app_id;
    char state[256] = "";
    char app_slug[48] = "";
    bool changed = false;
    bool sent = false;
    bool play_haptic = false;
};
