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
    Menu(SemaphoreHandle_t mutex);

    void initScreen();
    void add_page(int8_t id, int8_t app_id, const char *friendly_name, lv_img_dsc_t icon, lv_img_dsc_t small_icon);

    std::shared_ptr<MenuPage> find_page(int8_t id);

    uint8_t get_menu_position();
    void set_menu_position(uint8_t position);

    uint8_t get_menu_page_count();
    void set_menu_page_count(uint8_t count);

protected:
    lv_obj_t *left_image_icon;
    lv_obj_t *right_image_icon;
    lv_img_dsc_t left_icon;
    lv_img_dsc_t right_icon;

    uint8_t menu_page_count = 0;
    uint8_t current_menu_position = 0;

    std::map<uint8_t, std::shared_ptr<MenuPage>> pages;
};