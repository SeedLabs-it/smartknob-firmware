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

    void add_item(uint8_t id, MenuItem item);
    MenuItem *find_item(uint8_t id);
    std::string getClassName();
    std::pair<app_types, uint8_t> navigationNext();

    void lock();

private:
    uint8_t menu_items_count = 0;
    uint8_t current_menu_position = 0;
    std::map<uint8_t, MenuItem> items;
    char room[12];
    void render_menu_screen();

    SemaphoreHandle_t mutex;

    MenuItem *current_item;
    MenuItem *prev_item;
    MenuItem *next_item;

    void unlock();
};