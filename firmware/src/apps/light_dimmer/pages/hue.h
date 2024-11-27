
#pragma once
#include "./display/page_manager.h"
#include "./util.h"

#define skip_degrees_def 1

class HuePage : public BasePage
{
public:
    HuePage(lv_obj_t *parent);
    void update(xSemaphoreHandle mutex, int16_t position) override;

private:
    lv_obj_t *hue_wheel;
    lv_obj_t *hue_selector;
    lv_obj_t *hue_label;

    lv_color_hsv_t hsv = {.h = 0, .s = 100, .v = 100};

    const uint8_t skip_degrees = skip_degrees_def;
    const uint8_t skip_degrees_selectable = 4;
    const uint16_t lines_count = 360 / skip_degrees;
    const uint16_t lines_count_selectable = 360 / skip_degrees_selectable;
    const uint16_t distance_from_center = 70;
    const uint16_t line_length = 120;
    const float deg_1_rad = M_PI / 180.0;
    const uint16_t selector_radius = 96;
};
