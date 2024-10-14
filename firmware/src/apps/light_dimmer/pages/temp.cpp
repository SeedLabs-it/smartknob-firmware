#include "temp.h"

TempPage::TempPage(lv_obj_t *parent) : BasePage(parent)
{
    temp_selector = lvDrawCircle(9, page);
    lv_obj_set_style_bg_color(temp_selector, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_MAIN);
    lv_obj_align(temp_selector, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_style_bg_color(temp_selector, lv_color_hsv_to_rgb(app_hue_position * skip_degrees_selectable, 100, 100), LV_PART_MAIN);
    // lv_obj_align(temp_selector, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad * (270 + app_hue_position)), selector_radius * sin(deg_1_rad * (270 + app_hue_position)));
}

void TempPage::updateFromSystem(AppState state)
{
}

void TempPage::handleNavigation(NavigationEvent event)
{
}