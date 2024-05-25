#pragma once

#include "lvgl_app.h"
#include "./display/page_manager.h"
// #include "font/NDS1210pt7b.h"

#include <map>
#include <memory>

struct TextItem
{
    char *text;
    lv_color_t color;

    TextItem(char *text = "", lv_color_t color = lv_color_white()) : text(text), color(color){};
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

class MenuPage : public BasePage
{
public:
    MenuPage(lv_obj_t *parent, const char *friendly_name, lv_image_dsc_t icon, lv_image_dsc_t leftIcon, lv_image_dsc_t rightIcon) : BasePage(parent), friendly_name_(friendly_name), icon_(icon), leftIcon_(leftIcon), rightIcon_(rightIcon)
    {
        lv_obj_t *img = lv_img_create(page);
        lv_img_set_src(img, &icon_);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, friendly_name_);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 32);
        lv_obj_set_style_text_font(label, &NDS12_20px, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t *leftIconImg = lv_img_create(page);
        lv_img_set_src(leftIconImg, &leftIcon_);
        lv_obj_set_size(leftIconImg, 32, 32);
        lv_obj_align(leftIconImg, LV_ALIGN_CENTER, -50, 0);

        lv_obj_t *rightIconImg = lv_img_create(page);
        lv_img_set_src(rightIconImg, &rightIcon_);
        lv_obj_set_size(rightIconImg, 32, 32);
        lv_obj_align(rightIconImg, LV_ALIGN_CENTER, 50, 0);
        return;
    }

private:
    const char *friendly_name_;
    lv_image_dsc_t icon_;
    lv_image_dsc_t leftIcon_;
    lv_image_dsc_t rightIcon_;
};

class Menu : public App
{
public:
    Menu(SemaphoreHandle_t mutex) : App(mutex) { initScreen(); }
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) {}
    void updateStateFromSystem(AppState state) {};

    void initScreen()
    {
        // lv_obj_t *label = lv_label_create(screen);
        // lv_label_set_text(label, "Menu");
        // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        LV_IMAGE_DECLARE(lightbulb_regular);
        LV_IMAGE_DECLARE(logo_main_gradient_transparent);
        LV_IMAGE_DECLARE(logo_white_transparent);
        const char *friendly_name = "Menu";
        MenuPage *menuPage = new MenuPage(screen, friendly_name, lightbulb_regular, logo_main_gradient_transparent, logo_white_transparent);
        menuPage->show();
    };

    // TFT_eSprite *render() {};

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
        next_ = find_item(position)->app_id;
    };

    uint8_t get_menu_items_count() { return menu_items_count; };
    void set_menu_items_count(uint8_t count) { menu_items_count = count; };

protected:
    uint8_t menu_items_count = 0;
    uint8_t current_menu_position = 0;

    std::map<uint8_t, std::shared_ptr<MenuItem>> items;
};