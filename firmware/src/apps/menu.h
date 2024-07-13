#pragma once

#include "app.h"
#include "./display/page_manager.h"
// #include "font/NDS1210pt7b.h"

#include <map>
#include <memory>

class MenuPage : public BasePage
{
public:
    MenuPage(lv_obj_t *parent, int8_t app_id, const char *friendly_name, lv_img_dsc_t icon, lv_img_dsc_t small_icon) : BasePage(parent), app_id_(app_id), friendly_name_(friendly_name), icon_(icon), small_icon_(small_icon)
    {
        lv_obj_t *img = lv_img_create(page);
        lv_img_set_src(img, &icon_);
        // lv_obj_set_style_image_recolor_opa(img, LV_OPA_COVER, 0);
        lv_obj_set_style_img_recolor_opa(img, LV_OPA_COVER, 0);
        // lv_obj_set_style_image_recolor(img, LV_COLOR_MAKE(0x00, 0xFF, 0xFF), 0);
        lv_obj_set_style_img_recolor(img, LV_COLOR_MAKE(0x00, 0xFF, 0xFF), 0);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, friendly_name_);
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -48);
        lv_obj_set_style_text_font(label, &nds12_20px, 0);

        return;
    }

    lv_img_dsc_t *getSmallIcon() { return &small_icon_; }

    int8_t app_id_;

private:
    const char *friendly_name_;
    lv_img_dsc_t icon_;
    lv_img_dsc_t small_icon_;
};

class Menu : public App
{
public:
    Menu(SemaphoreHandle_t mutex) : App(mutex)
    {
        initScreen();
    };

    void initScreen()
    {
        static lv_style_t style;
        lv_style_init(&style);
        // lv_style_set_image_recolor_opa(&style, LV_OPA_COVER);
        lv_style_set_img_recolor_opa(&style, LV_OPA_COVER);
        // lv_style_set_image_recolor(&style, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));
        lv_style_set_img_recolor(&style, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));

        lv_obj_t *overlay = lv_obj_create(screen);
        lv_obj_remove_style_all(overlay);
        lv_obj_set_size(overlay, LV_HOR_RES, LV_VER_RES);

        lv_obj_t *label = lv_label_create(overlay);
        lv_label_set_text(label, "Menu");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 32);

        left_image_icon = lv_img_create(overlay);
        lv_obj_add_style(left_image_icon, &style, 0);
        lv_obj_align(left_image_icon, LV_ALIGN_LEFT_MID, 20, 6);

        right_image_icon = lv_img_create(overlay);
        lv_obj_add_style(right_image_icon, &style, 0);
        lv_obj_align(right_image_icon, LV_ALIGN_RIGHT_MID, -20, 6);
    };

    void add_page(int8_t id, int8_t app_id, const char *friendly_name, lv_img_dsc_t icon, lv_img_dsc_t small_icon)
    {
        pages[id] = std::make_shared<MenuPage>(screen, app_id, friendly_name, icon, small_icon);
        menu_page_count++;
    };

    // std::shared_ptr<MenuItem> find_item(int8_t id) { return items[id]; };
    std::shared_ptr<MenuPage> find_page(int8_t id) { return pages[id]; };

    uint8_t get_menu_position() { return current_menu_position; };
    void set_menu_position(uint8_t position)
    {
        current_menu_position = position;
        next_ = find_page(position)->app_id_;
    };

    uint8_t get_menu_page_count() { return menu_page_count; };
    void set_menu_page_count(uint8_t count) { menu_page_count = count; };

protected:
    lv_obj_t *left_image_icon;
    lv_obj_t *right_image_icon;
    lv_img_dsc_t left_icon;
    lv_img_dsc_t right_icon;

    uint8_t menu_page_count = 0;
    uint8_t current_menu_position = 0;

    // std::map<uint8_t, std::shared_ptr<MenuItem>> items;
    std::map<uint8_t, std::shared_ptr<MenuPage>> pages;
};