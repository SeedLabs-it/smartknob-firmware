#include "page_selector.h"

PageSelector::PageSelector(lv_obj_t *parent) : BasePage(parent)
{
    hue_picker_label_ = lv_label_create(page);
    lv_label_set_text(hue_picker_label_, "HUE PICKER");
    lv_obj_align(hue_picker_label_, LV_ALIGN_CENTER, 0, -24);

    temp_picker_label_ = lv_label_create(page);
    lv_label_set_text(temp_picker_label_, "TEMP PICKER");
    lv_obj_align_to(temp_picker_label_, hue_picker_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);

    // presets_picker_label_ = lv_label_create(page);
    // lv_label_set_text(presets_picker_label_, "PRESETS PICKER");
    // lv_obj_align_to(presets_picker_label_, temp_picker_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);

    selector_ = lvDrawCircle(9, page);
    lv_obj_set_style_bg_color(selector_, LV_COLOR_MAKE(0x78, 0xDB, 0x89), LV_PART_MAIN);
    lv_obj_set_size(selector_, lv_obj_get_width(hue_picker_label_) + 12, lv_obj_get_height(hue_picker_label_));
    lv_obj_align_to(selector_, hue_picker_label_, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_move_background(selector_);
}

void PageSelector::update(xSemaphoreHandle mutex, int16_t current_position)
{
    switch (current_position)
    {
    case 0:
        lv_obj_set_size(selector_, lv_obj_get_width(hue_picker_label_) + 12, lv_obj_get_height(hue_picker_label_));
        lv_obj_align_to(selector_, hue_picker_label_, LV_ALIGN_BOTTOM_MID, 0, 0);
        break;
    case 1:
        lv_obj_set_size(selector_, lv_obj_get_width(temp_picker_label_) + 12, lv_obj_get_height(temp_picker_label_));
        lv_obj_align_to(selector_, temp_picker_label_, LV_ALIGN_BOTTOM_MID, 0, 0);
        break;
    // case 2:
    //     lv_obj_set_size(selector_, lv_obj_get_width(presets_picker_label_) + 12, lv_obj_get_height(presets_picker_label_));
    //     lv_obj_align_to(selector_, presets_picker_label_, LV_ALIGN_BOTTOM_MID, 0, 0);
    //     break;
    default:
        break;
    }
}