#pragma once

#include "../menu.h"
#include "font/NDS1210pt7b.h"

#include <map>

class OnboardingMenu : public Menu
{

public:
    OnboardingMenu(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

    void add_item(int8_t id, std::shared_ptr<MenuItem> item);

private:
};
