
#pragma once
#include "./display/page_manager.h"
#include "./util.h"

#define temp_min 1000
#define temp_max 20000
#define temp_1_deg (temp_max - temp_min) / 360

#define skip_degrees_def 1 // distance between two lines in degrees [TODO] refactor this should be the space between lines, not the distance between the start of a line and the other (To account of >1px line)
// #define skip_degrees_selectable 4        // distance between two lines in degrees [TODO] refactor this should be the space between lines, not the distance between the start of a line and the other (To account of >1px line)
// #define lines_count (360 / skip_degrees) // number of lines in a 360 circle. // [TODO] refactor, this should account of line thickness + space_between_lines.
// #define lines_count_selectable (360 / skip_degrees_selectable)
// #define distance_from_center 70 // distance from center (pixel) from where the line starts (and goes outword towards the edge of the display)
// #define line_length 120         // length of the ticker line (in pixel)
// #define deg_1_rad (M_PI / 180.0)
// #define selector_radius 80 // Radius of where to place selector dot.

class TempPage : public BasePage
{
public:
    TempPage(lv_obj_t *parent);
    void update(int16_t position) override;

private:
    lv_obj_t *temp_wheel;
    lv_obj_t *temp_selector;
    lv_obj_t *temp_label;

    lv_color_hsv_t hsv;

    const uint8_t skip_degrees = skip_degrees_def;
    const uint8_t skip_degrees_selectable = 4;
    const uint16_t lines_count = 360 / skip_degrees;
    const uint16_t lines_count_selectable = 360 / skip_degrees_selectable;
    const uint16_t distance_from_center = 70;
    const uint16_t line_length = 120;
    const float deg_1_rad = M_PI / 180.0;
    const uint16_t selector_radius = 80;
};
