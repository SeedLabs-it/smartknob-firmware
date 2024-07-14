#include "menu.h"

Menu::Menu(SemaphoreHandle_t mutex) : App(mutex)
{
    initScreen();
};

void Menu::initScreen()
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_img_recolor_opa(&style, LV_OPA_COVER);
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

void Menu::add_page(int8_t id, int8_t app_id, const char *friendly_name, lv_img_dsc_t icon, lv_img_dsc_t small_icon)
{
    pages[id] = std::make_shared<MenuPage>(screen, app_id, friendly_name, icon, small_icon);
    menu_page_count++;
};

std::shared_ptr<MenuPage> Menu::find_page(int8_t id) { return pages[id]; };

uint8_t Menu::get_menu_position() { return current_menu_position; };
void Menu::set_menu_position(uint8_t position)
{
    current_menu_position = position;
    next_ = find_page(position)->app_id_;
};

uint8_t Menu::get_menu_page_count() { return menu_page_count; };
void Menu::set_menu_page_count(uint8_t count) { menu_page_count = count; };