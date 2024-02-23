#pragma once
#include "../app.h"
#include "../../util.h"
#include "../../navigation/navigation.h"
#include "../../notify/motor_notifier/motor_notifier.h"
#include "../../notify/wifi_notifier/wifi_notifier.h"
#include "../../events/events.h"

#include "../../display_buffer.h"

// Fonts
#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"
#include "../../font/NDS1210pt7b.h"
#include "../../font/NDS125_small.h"
#include "../../font/Pixel62mr11pt7b.h"

// TODO make this enum ?

const uint8_t ONBOARDING_FLOW_PAGE_STEP_WELCOME = 0;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_1 = 1;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_WIFI_1 = 2;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_DEMO_1 = 3;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_ABOUT = 4;

const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_2 = 5;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_3 = 6;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_4 = 7;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_5 = 8;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_6 = 9;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_7 = 10;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_8 = 11;

class OnboardingFlow
{
public:
    OnboardingFlow();
#ifdef USE_DISPLAY_BUFFER
    void render();
#else
    OnboardingFlow(TFT_eSprite *spr_);
    TFT_eSprite *render();
#endif

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    EntityStateUpdate update(AppState state);
    void handleNavigationEvent(NavigationEvent event);
    void handleWiFiEvent(WiFiEvent event);
    void setMotorUpdater(MotorNotifier *motor_notifier);
    void setWiFiNotifier(WiFiNotifier *wifi_notifier);
    void triggerMotorConfigUpdate();

private:
#ifdef USE_DISPLAY_BUFFER
    void renderWelcomePage();
    void renderHass1StepPage();
    void renderHass2StepPage();
    void renderHass3StepPage();
    void renderHass4StepPage();
    void renderHass5StepPage();
    void renderHass6StepPage();
    void renderHass7StepPage();
    void renderHass8StepPage();
    void renderWiFi1StepPage();
    void renderDemo1StepPage();
    void renderAboutPage();
#else
    TFT_eSprite *renderWelcomePage();
    TFT_eSprite *renderHass1StepPage();
    TFT_eSprite *renderHass2StepPage();
    TFT_eSprite *renderHass3StepPage();
    TFT_eSprite *renderHass4StepPage();
    TFT_eSprite *renderHass5StepPage();
    TFT_eSprite *renderHass6StepPage();
    TFT_eSprite *renderHass7StepPage();
    TFT_eSprite *renderHass8StepPage();
    TFT_eSprite *renderWiFi1StepPage();
    TFT_eSprite *renderDemo1StepPage();
    TFT_eSprite *renderAboutPage();
#endif
    int32_t current_position = 0;

    uint8_t current_page = 0;

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config;

    MotorNotifier *motor_notifier;
    WiFiNotifier *wifi_notifier;

    char buf_[64];

    // UI
    TFT_eSprite *spr_ = NULL;

    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);

    // wifi AP variables
    bool is_wifi_ap_started = false;
    char wifi_ap_ssid[12];
    char wifi_ap_passphrase[9];
    bool is_wifi_ap_client_connected = false;

    // web progress variables
    bool is_web_client_connected = false;
    bool is_sta_connecting = false;
    uint8_t sta_connecting_tick = 0;
    char wifi_sta_ssid[128];
    char wifi_sta_passphrase[128];
    char wifi_qr_code[40];
    char mqtt_server[32];
    uint8_t mqtt_connecting_tick = 0;
};