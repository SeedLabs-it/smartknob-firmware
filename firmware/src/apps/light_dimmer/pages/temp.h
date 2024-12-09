
#pragma once
#include "./display/page_manager.h"
#include "./util.h"
#include <cmath>

// TODO: Retrieve from HASS entity.
#define temp_max 2000 // Philips hue min
#define temp_min 6500 // Philips hue max

#define temp_1_deg (temp_max - temp_min) / 360

#define skip_degrees_def 1

class TempPage : public BasePage
{
public:
    TempPage(lv_obj_t *parent, const AppData &app_data);
    void update(xSemaphoreHandle mutex, int16_t position) override;

private:
    lv_obj_t *temp_wheel;
    lv_obj_t *temp_selector;
    lv_obj_t *temp_label;
    lv_obj_t *selected_temp_circle;
    lv_obj_t *friendly_name_label_;

    lv_color_hsv_t hsv;

    const uint8_t skip_degrees = skip_degrees_def;
    const uint8_t skip_degrees_selectable = 4;
    const uint16_t lines_count = 360 / skip_degrees;
    const uint16_t lines_count_selectable = 360 / skip_degrees_selectable;
    const uint16_t distance_from_center = 70;
    const uint16_t line_length = 120;
    const float deg_1_rad = M_PI / 180.0;
    const uint16_t selector_radius = 96;
};
