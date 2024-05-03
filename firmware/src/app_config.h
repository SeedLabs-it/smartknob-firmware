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
    char ssid[128];
    char ip_address[20];

    bool is_ap;
    IPAddress ap_ip_address;
    bool ap_has_clients;
};

struct MqttState
{
    bool is_connected;
    std::string server;
    std::string client_id;
};

struct IlluminationState
{
    float lux;
    float lux_avg;
    float lux_adj; // a value between 0 and 1.
};

// TODO: rename fields to follow the convention
struct ProximityState
{
    uint16_t RangeMilliMeter;
    uint8_t RangeStatus;
};

struct StrainState
{
    float raw_value;
    float press_value;
    uint8_t virtual_button_code;
};

struct SystemState
{
    float esp32_temperature;
};

struct SensorsState
{
    ProximityState proximity;
    StrainState strain;
    IlluminationState illumination;
    SystemState system;
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
    MqttState mqtt_state;
    ProximityState proximiti_state;
    ScreenState screen_state;
    cJSON *apps;
};

struct EntityStateUpdate
{
    char app_id[256] = "";
    char entity_id[256] = "";
    char state[256] = "";
    char app_slug[48] = "";
    bool changed = false;
    bool sent = false;
    bool acknowledged = false;
    bool play_haptic = false;
};
