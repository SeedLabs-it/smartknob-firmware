#pragma once

#include "app.h"
#include "font/NDS1210pt7b.h"

#include <map>

struct TextItem
{
    const char *text;
    uint16_t color;

    TextItem(const char *text = "", uint16_t color = 0) : text(text), color(color){};
};

struct IconItem
{
    const unsigned char *icon;
    uint16_t color;

    IconItem(const unsigned char *icon = nullptr, uint16_t color = 0) : icon(icon), color(color){};
};

struct MenuItem
{
    uint8_t app_id;
    TextItem screen_name;
    TextItem screen_description;
    TextItem call_to_action;
    IconItem big_icon;
    IconItem small_icon;
};

class Menu : public App
{
public:
    Menu(TFT_eSprite *spr_) : App(spr_){};
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) {};
    void updateStateFromSystem(AppState state) {};

    TFT_eSprite *render() {};

    void add_item(uint8_t id, MenuItem item)
    {
        items[id] = item;
        motor_config.max_position = menu_items_count;
        menu_items_count++;
    };
    MenuItem find_item(uint8_t id) { return items[id]; };

    uint8_t get_menu_position() { return current_menu_position; };
    void set_menu_position(uint8_t position)
    {
        current_menu_position = position;
        next = find_item(position).app_id;
    };

    uint8_t get_menu_items_count() { return menu_items_count; };
    void set_menu_items_count(uint8_t count) { menu_items_count = count; };

private:
    uint8_t menu_items_count = 0;
    uint8_t current_menu_position = 0;

    std::map<uint8_t, MenuItem> items;
};