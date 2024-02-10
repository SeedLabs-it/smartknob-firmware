#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"

class OnboardingFlow : public App
{
public:
    OnboardingFlow(TFT_eSprite *spr_, std::string entity_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

private:
    std::string entity_name;
    uint8_t current_volume = 0;
    uint8_t last_volume = 0;
    uint8_t current_volume_position = 0;
};