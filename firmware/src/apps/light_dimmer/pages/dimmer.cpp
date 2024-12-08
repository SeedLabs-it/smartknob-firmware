#include "dimmer.h"

DimmerPage::DimmerPage(lv_obj_t *parent, const AppData &app_data) : BasePage(parent)
{
    static lv_style_t style; // Needed as bg is removed in BasePage
    lv_style_init(&style);

    lv_style_set_bg_color(&style, LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_obj_add_style(page, &style, LV_PART_MAIN);

    arc_ = lv_arc_create(page);
    lv_obj_set_size(arc_, 220, 220);
    lv_arc_set_rotation(arc_, 150);
    lv_arc_set_bg_angles(arc_, 0, 240);
    lv_arc_set_value(arc_, 0);
    lv_obj_center(arc_);

    lv_obj_set_style_bg_color(arc_, dark_arc_bg, LV_PART_KNOB);
    lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_, LV_COLOR_MAKE(0xF5, 0xA4, 0x42), LV_PART_INDICATOR);

    lv_obj_set_style_arc_width(arc_, 18, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, 18, LV_PART_INDICATOR);
    lv_obj_set_style_pad_all(arc_, -6, LV_PART_KNOB);

    percentage_label_ = lv_label_create(page);
    char buf_[16];
    // sprintf(buf_, "%d%%", current_brightness);
    sprintf(buf_, "%d%%", 0);
    lv_label_set_text(percentage_label_, buf_);
    lv_obj_set_style_text_font(percentage_label_, &roboto_light_mono_48pt, 0);
    lv_obj_align(percentage_label_, LV_ALIGN_CENTER, 0, -12);

    friendly_name_label_ = lv_label_create(page);
    lv_label_set_text(friendly_name_label_, app_data.friendly_name);
    lv_obj_align_to(friendly_name_label_, percentage_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
}

void DimmerPage::update(xSemaphoreHandle mutex, int16_t position)
{
    SemaphoreGuard lock(mutex);
    // current_position_ = state.current_position;
    if (position == 0)
    {
        lv_obj_set_style_bg_color(page, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_PART_MAIN);
        lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
    }
    else if (lv_obj_get_style_bg_color(page, LV_PART_MAIN).full != (lv_color_t)LV_COLOR_MAKE(0x47, 0x27, 0x01).full)
    {
        lv_obj_set_style_bg_color(page, LV_COLOR_MAKE(0x47, 0x27, 0x01), LV_PART_MAIN);
        lv_obj_set_style_arc_color(arc_, lv_color_mix(dark_arc_bg, LV_COLOR_MAKE(0x47, 0x27, 0x01), 128), LV_PART_MAIN);
    }

    // if (color_set)
    // {
    //     lv_obj_set_style_arc_color(arc_, lv_color_hsv_to_rgb(app_hue_position * skip_degrees_selectable, 100, 100), LV_PART_INDICATOR);
    // }

    lv_arc_set_value(arc_, position);
    char buf_[16];
    sprintf(buf_, "%d%%", position);
    lv_label_set_text(percentage_label_, buf_);
}

void DimmerPage::handleNavigation(NavigationEvent event)
{
    LOGE("DimmerPage::handleNavigation");
}