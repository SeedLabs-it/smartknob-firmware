#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"

#include "../../font/NDS1210pt7b.h"
#include "../../font/Pixel62mr11pt7b.h"

#include "../../util.h"

class OnboardingFlow
{
public:
    OnboardingFlow();
    OnboardingFlow(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    EntityStateUpdate update(AppState state);

private:
    int32_t current_position = 0;

    TFT_eSprite *spr_;
    PB_SmartKnobConfig motor_config;

    char buf_[64];

    // UI constants
    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);

    TFT_eSprite *renderWelcomeScreen();
    TFT_eSprite *renderHass1StepScreen();
    TFT_eSprite *renderHass2StepScreen();
    TFT_eSprite *renderHass3StepScreen();
    TFT_eSprite *renderHass4StepScreen();
    TFT_eSprite *renderHass5StepScreen();
    TFT_eSprite *renderHass6StepScreen();
    TFT_eSprite *renderHass7StepScreen();
    TFT_eSprite *renderWiFi1StepScreen();
    TFT_eSprite *renderDemo1StepScreen();
    TFT_eSprite *renderAboutScreen();
};