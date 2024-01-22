#pragma once
#include "app.h"
#include "font/roboto_thin_bold_24.h"

#include <map>

const uint8_t SCREEN_NAME_LENGTH = 20;
const uint8_t MEX_MENU_ITEMS = 12;

struct MenuItemOld
{
    const char *screen_name;
    uint16_t app_id;
    uint32_t color;
    const unsigned char *small_icon;
    const unsigned char *big_icon;
};

class MenuApp : public App
{
public:
    MenuApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

    void add_item(uint8_t id, MenuItemOld item);
    MenuItemOld *find_item(uint8_t id);
    std::string getClassName();

    uint8_t navigationNext();
    uint8_t navigationBack();

    void lock();

private:
    uint8_t menu_items_count = 0;
    uint8_t current_menu_position = 0;
    std::map<uint8_t, MenuItemOld> items;
    char room[12];
    void render_menu_screen();

    SemaphoreHandle_t mutex;

    MenuItemOld *current_item;
    MenuItemOld *prev_item;
    MenuItemOld *next_item;

    void unlock();
};