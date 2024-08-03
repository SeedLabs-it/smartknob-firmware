#include "./error.h"

ErrorPage::ErrorPage(lv_obj_t *parent) : BasePage(parent)
{
    error_label = lv_label_create(page);
    lv_label_set_text(error_label, "ERROR");
    lv_obj_set_style_text_color(error_label, LV_COLOR_MAKE(0xED, 0x43, 0x37), LV_PART_MAIN);
    lv_obj_align(error_label, LV_ALIGN_TOP_MID, 0, 10);

    error_type_label = lv_label_create(page);
    lv_obj_align(error_type_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_text_font(error_type_label, &nds12_20px, 0);

    error_event_label = lv_label_create(page);
    lv_obj_set_style_text_color(error_event_label, LV_COLOR_MAKE(0xED, 0x43, 0x37), LV_PART_MAIN);
    lv_obj_align_to(error_event_label, error_type_label, LV_ALIGN_OUT_TOP_MID, 0, -10);
}

void ErrorPage::setErrorTypeLabel(const char *label)
{
    lv_label_set_text(error_type_label, label);
    lv_obj_align(error_type_label, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void ErrorPage::setErrorEventLabel(const char *label)
{
    lv_label_set_text(error_event_label, label);
    lv_obj_align_to(error_event_label, error_type_label, LV_ALIGN_OUT_TOP_MID, 0, -10);
}
