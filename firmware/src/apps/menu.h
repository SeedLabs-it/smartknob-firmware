#pragma once

#include "app.h"
#include "font/NDS1210pt7b.h"

#include <map>
#include <memory>

#include "display_buffer.h"

struct TextItem
{
    char *text;
    uint16_t color;

    TextItem(char *text = "", uint16_t color = 0) : text(text), color(color){};
};

struct IconItem
{
    const unsigned char *icon;
    uint16_t color;

    IconItem(const unsigned char *icon = nullptr, uint16_t color = 0) : icon(icon), color(color){};
};

struct MenuItem
{
    int8_t app_id;
    TextItem screen_name;
    TextItem screen_description;
    TextItem call_to_action;
    IconItem big_icon;
    IconItem small_icon;

    MenuItem(
        int8_t app_id = -1,
        TextItem screen_name = TextItem{},
        TextItem screen_description = TextItem{},
        TextItem call_to_action = TextItem{},
        IconItem big_icon = IconItem{},
        IconItem small_icon = IconItem{})
        : app_id(app_id),
          screen_name(screen_name),
          screen_description(screen_description),
          call_to_action(call_to_action),
          big_icon(big_icon),
          small_icon(small_icon){};
};

class Menu : public App
{
public:
    #ifdef USE_DISPLAY_BUFFER
        Menu() : App(){};
        void render(){};
    #else
        Menu(TFT_eSprite *spr_) : App(spr_){};
        TFT_eSprite *render(){};
    #endif
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state){};
    void updateStateFromSystem(AppState state){};

    virtual void add_item(int8_t id, std::shared_ptr<MenuItem> item)
    {
        items[id] = item;
        menu_items_count++;
    };

    std::shared_ptr<MenuItem> find_item(int8_t id) { return items[id]; };

    uint8_t get_menu_position() { return current_menu_position; };
    void set_menu_position(uint8_t position)
    {
        current_menu_position = position;
        next = find_item(position)->app_id;
    };

    uint8_t get_menu_items_count() { return menu_items_count; };
    void set_menu_items_count(uint8_t count) { menu_items_count = count; };

protected:
    uint8_t menu_items_count = 0;
    uint8_t current_menu_position = 0;

    std::map<uint8_t, std::shared_ptr<MenuItem>> items;
};