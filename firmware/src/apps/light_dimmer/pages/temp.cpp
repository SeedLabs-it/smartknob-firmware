#include "temp.h"

static lv_obj_t *meter;

static void meter_draw_event_cb(lv_event_t *e)
{
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
    if (dsc->type == LV_METER_DRAW_PART_TICK)
    {
        if (dsc->id >= 180)
        {
            dsc->line_dsc->color = kelvinToLvColor(temp_max - dsc->id * skip_degrees_def * temp_1_deg);
        }
        else
        {
            dsc->line_dsc->color = kelvinToLvColor(temp_min + dsc->id * skip_degrees_def * temp_1_deg);
        }
    }
}

TempPage::TempPage(lv_obj_t *parent) : BasePage(parent)
{
    meter = lv_meter_create(page);
    lv_obj_remove_style_all(meter);
    lv_obj_set_size(meter, 210, 210);
    lv_obj_center(meter);

    lv_meter_scale_t *scale_temp = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_temp, lines_count - 1, 3, 12, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_temp, 0, lines_count, 360, 270);

    // indic_hue = lv_meter_add_needle_line(meter, scale_hue, 2, LV_COLOR_MAKE(0x00, 0x00, 0x00), -20);

    lv_obj_add_event_cb(meter, meter_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, nullptr);

    temp_selector = lvDrawCircle(9, page);
    lv_obj_set_style_bg_color(temp_selector, kelvinToLvColor(1000), LV_PART_MAIN);
    lv_obj_align(temp_selector, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad * (270 + 0)), selector_radius * sin(deg_1_rad * (270 + 0)));
}

void TempPage::updateFromSystem(AppState state)
{
}

void TempPage::handleNavigation(NavigationEvent event)
{
}