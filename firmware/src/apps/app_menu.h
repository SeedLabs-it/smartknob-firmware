#pragma once
#include "menu.h"
#include "font/roboto_thin_bold_24.h"

#include <map>

const uint8_t SCREEN_NAME_LENGTH = 20;
const uint8_t MEX_MENU_ITEMS = 12;

class MenuApp : public Menu
{
public:
    MenuApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

    void add_item(int8_t id, MenuItem item);

private:
    char room[12];

    MenuItem current_item;
    MenuItem prev_item;
    MenuItem next_item;
};