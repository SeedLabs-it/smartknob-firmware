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

    uint8_t navigationNext();
    uint8_t navigationBack();

    TFT_eSprite *render();

private:
};
