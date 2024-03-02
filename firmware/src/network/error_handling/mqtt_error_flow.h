#pragma once
#include "apps/app.h"

#include "util.h"
#include "navigation/navigation.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"

#include "font/NDS1210pt7b.h"
#include "font/NDS125_small.h"

enum ErrorMode
{
    NO_ERROR,
    MQTT_ERROR,
};

class MqttErrorFlow
{
public:
    MqttErrorFlow();
    MqttErrorFlow(TFT_eSprite *spr_);

    TFT_eSprite *render();
    // EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    // void updateStateFromSystem(AppState state);
    // EntityStateUpdate update(AppState state);
    void handleNavigationEvent(NavigationEvent event);
    void handleEvent(WiFiEvent event);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void setWiFiNotifier(WiFiNotifier *wifi_notifier);

private:
    TFT_eSprite *spr_ = NULL;

    char buf_[64];

    int32_t current_position = 0;
    WiFiEvent latest_event;
    uint32_t latest_event_received_at = 0;
    ErrorMode error_mode = NO_ERROR;

    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        "",
        0,
        {},
        0,
        248,
    };

    MotorNotifier *motor_notifier;
    WiFiNotifier *wifi_notifier;

    TFT_eSprite *renderMqttConnectionFailed();
    TFT_eSprite *renderMqttRetryLimitReached();
};