#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"

#include "../../font/NDS1210pt7b.h"
#include "../../font/Pixel62mr11pt7b.h"

#include "../../util.h"

#include "../../navigation/navigation.h"

// TODO make this enum ?

const uint8_t ONBOARDING_FLOW_PAGE_STEP_WELCOME = 0;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_1 = 1;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_WIFI_1 = 2;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_DEMO_1 = 3;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_ABOUT = 4;

const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_2 = 5;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_3 = 6;
const uint8_t ONBOARDING_FLOW_PAGE_STEP_HASS_4 = 7;

class OnboardingFlow
{
public:
    OnboardingFlow();
    OnboardingFlow(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    EntityStateUpdate update(AppState state);

    void handleNavigationEvent(NavigationEvent event);

private:
    int32_t current_position = 0;

    uint8_t current_page = 0;

    TFT_eSprite *spr_;
    PB_SmartKnobConfig motor_config;

    char buf_[64];

    // UI constants
    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);

    TFT_eSprite *renderWelcomePage();
    TFT_eSprite *renderHass1StepPage();
    TFT_eSprite *renderHass2StepPage();
    TFT_eSprite *renderHass3StepPage();
    TFT_eSprite *renderHass4StepPage();
    TFT_eSprite *renderHass5StepPage();
    TFT_eSprite *renderHass6StepPage();
    TFT_eSprite *renderHass7StepPage();
    TFT_eSprite *renderWiFi1StepPage();
    TFT_eSprite *renderDemo1StepPage();
    TFT_eSprite *renderAboutPage();
};