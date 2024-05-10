#pragma once
#include "apps/app.h"

// #include "util.h"
// #include "qrcode.h"
// #include "navigation/navigation.h"
// #include "events/events.h"

// // Fonts
// #include "font/roboto_thin_bold_24.h"
// #include "font/roboto_thin_20.h"
// #include "font/NDS125_small.h"
// #include "font/Pixel62mr11pt7b.h"
#include "lvgl.h"
#include "util.h"
#include "logging.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"
#include "notify/os_config_notifier/os_config_notifier.h"
// #include "./assets/images/skdk/logo"

class OnboardingFlow

{
public:
    OnboardingFlow();

    void render();

    EntityStateUpdate update(AppState state);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);

    void setWiFiNotifier(WiFiNotifier *wifi_notifier);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void triggerMotorConfigUpdate();

private:
    uint8_t current_position = 0;
    char firmware_version[16];

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config;

    WiFiNotifier *wifi_notifier;
    OSConfigNotifier *os_config_notifier;
    MotorNotifier *motor_notifier;

    void welcomeScreenInit();
    void hassScreenInit();
    void wifiScreenInit();
    void demoScreenInit();
    void aboutScreenInit();

    enum OnboardingFlowPage
    {
        WELCOME = 0,
        HASS_1 = 1,
        WIFI_1 = 2,
        DEMO_1 = 3,
        ABOUT = 4,
        ONBOARDING_FLOW_PAGE_COUNT
    };

    lv_obj_t *screens[ONBOARDING_FLOW_PAGE_COUNT];
};

// // TODO make this enum ?

// const uint8_t ONBOARDING_FLOW_PAGE_STEP_WELCOME = 0;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_1 = 1;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_WIFI_1 = 2;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_DEMO_1 = 3;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_ABOUT = 4;

// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_2 = 5;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_3 = 6;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_4 = 7;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_5 = 8;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_6 = 9;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_7 = 10;
// const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_8 = 11;

// class OnboardingFlow
// {
// public:
//     OnboardingFlow();
// void setQRCode(char *qr_data);
//     void updateStateFromSystem(AppState state);
//     EntityStateUpdate update(AppState state);
//     void handleNavigationEvent(NavigationEvent event);
//     void handleEvent(WiFiEvent event);
//     void triggerMotorConfigUpdate();

// private:
//     // TFT_eSprite *renderWelcomePage();
//     // TFT_eSprite *renderHass1StepPage();
//     // TFT_eSprite *renderHass2StepPage();
//     // TFT_eSprite *renderHass3StepPage();
//     // TFT_eSprite *renderHass4StepPage();
//     // TFT_eSprite *renderHass5StepPage();
//     // TFT_eSprite *renderHass6StepPage();
//     // TFT_eSprite *renderHass7StepPage();
//     // TFT_eSprite *renderHass8StepPage();
//     // TFT_eSprite *renderWiFi1StepPage();
//     // TFT_eSprite *renderDemo1StepPage();
//     // TFT_eSprite *renderAboutPage();

//     char firmware_version[16];

//     int32_t current_position = 0;
//     uint8_t current_page = 0;

//     PB_SmartKnobConfig root_level_motor_config;
//     PB_SmartKnobConfig blocked_motor_config;

//     MotorNotifier *motor_notifier;
//     WiFiNotifier *wifi_notifier;
//     OSConfigNotifier *os_config_notifier;

//     WiFiEvent latest_event;

//     char buf_[64];

//     // UI
//     // TFT_eSprite *spr_ = NULL;
//     // TFT_eSprite qrcode_spr_;

//     uint16_t default_text_color = rgbToUint32(150, 150, 150);
//     uint16_t accent_text_color = rgbToUint32(128, 255, 80);

//     bool is_onboarding_finished = false;
//     bool new_wifi_credentials_failed = false;
//     bool new_mqtt_credentials_failed = false;

//     // wifi AP variables
//     bool is_wifi_ap_started = false;
//     char wifi_ap_ssid[12];
//     char wifi_ap_passphrase[9];
//     bool is_wifi_ap_client_connected = false;

//     // web progress variables
//     bool is_web_client_connected = false;
//     bool is_sta_connecting = false;
//     uint8_t sta_connecting_tick = 0;
//     char wifi_sta_ssid[128];
//     char wifi_sta_passphrase[128];
//     char ap_data[64];
//     char ip_data[64];
//     char mqtt_server[32];
//     uint8_t mqtt_connecting_tick = 0;
// };