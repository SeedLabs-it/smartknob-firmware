#include "hue.h"

static lv_obj_t *meter;

static void meter_draw_event_cb(lv_event_t *e)
{
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
    lv_color_hsv_t *hsv = (lv_color_hsv_t *)lv_event_get_user_data(e);
    if (dsc->type == LV_METER_DRAW_PART_TICK)
    {
        dsc->line_dsc->color = lv_color_hsv_to_rgb(dsc->id * skip_degrees_def, 100, 100);
    }
}

HuePage::HuePage(lv_obj_t *parent) : BasePage(parent)
{
    meter = lv_meter_create(page);
    lv_obj_remove_style_all(meter);
    lv_obj_set_size(meter, 210, 210);
    lv_obj_center(meter);

    lv_meter_scale_t *scale_hue = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hue, lines_count - 1, 3, 12, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_hue, 0, lines_count, 360, 270);

    // indic_hue = lv_meter_add_needle_line(meter, scale_hue, 2, LV_COLOR_MAKE(0x00, 0x00, 0x00), -20);

    lv_obj_add_event_cb(meter, meter_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, &hsv);

    hue_selector = lvDrawCircle(9, page);
    lv_obj_set_style_bg_color(hue_selector, lv_color_hsv_to_rgb(0 * skip_degrees_selectable, 100, 100), LV_PART_MAIN);
    lv_obj_align(hue_selector, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad * (270 + 0)), selector_radius * sin(deg_1_rad * (270 + 0)));
}

void HuePage::updateFromSystem(AppState state)
{
}

void HuePage::handleNavigation(NavigationEvent event)
{
}