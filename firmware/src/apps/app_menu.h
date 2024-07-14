#pragma once
#include "menu.h"

#include <map>

const uint8_t SCREEN_NAME_LENGTH = 20;
const uint8_t MEX_MENU_ITEMS = 12;

class MenuApp : public Menu
{
public:
    MenuApp(SemaphoreHandle_t mutex);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state) {};

    void update();

    void render();

private:
    char room[12];

    std::shared_ptr<MenuPage> current_page;
    std::shared_ptr<MenuPage> prev_page;
    std::shared_ptr<MenuPage> next_page;
};