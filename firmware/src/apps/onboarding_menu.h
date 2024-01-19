#pragma once

#include "menu.h"
#include "font/NDS1210pt7b.h"

#include <map>

class OnboardingMenu : public Menu
{

public:
    OnboardingMenu(TFT_eSprite *spr_);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    std::pair<app_types, uint8_t> navigationNext();
    TFT_eSprite *render();

private:
};
