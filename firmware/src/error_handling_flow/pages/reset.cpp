#include "reset.h"

void reset_timer(lv_timer_t *timer)
{
    ResetPage *reset_page = (ResetPage *)timer->user_data;

    lv_obj_t *reset_msg_label = reset_page->getResetMsgLabel();
    lv_obj_t *page = reset_page->getPage();

    uint8_t held_for = (int)((millis() - reset_page->start_ms) / 1000);
    if (held_for > SOFT_RESET_SECONDS)
    {
        lv_obj_set_style_bg_color(page, LV_COLOR_MAKE(0xFF, 0x00, 0x00), LV_PART_MAIN);
        lv_label_set_text_fmt(reset_msg_label, "FOR FACTORY RESET\nRELEASE IN %ds \nRELEASE FOR SOFT RESET", max(0, HARD_RESET_SECONDS - held_for));
        lv_obj_align(reset_msg_label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
    else
    {
        lv_obj_set_style_bg_color(page, LV_COLOR_MAKE(0xFF, 0x6E, 0x00), LV_PART_MAIN);
        lv_label_set_text_fmt(reset_msg_label, "FOR SOFT RESET\nRELEASE IN %ds \nRELEASE TO CANCEL", max(0, SOFT_RESET_SECONDS - held_for));
        lv_obj_align(reset_msg_label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
}

ResetPage::ResetPage(lv_obj_t *parent) : BasePage(parent)
{
    reset_label = lv_label_create(page);
    lv_label_set_text(reset_label, "RESET");
    lv_obj_align(reset_label, LV_ALIGN_TOP_MID, 0, 10);

    reset_msg_label = lv_label_create(page);
    lv_obj_set_style_text_align(reset_msg_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(reset_msg_label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);

    lv_obj_set_style_bg_opa(page, LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_bg_color(page, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_PART_MAIN);
}

void ResetPage::show()
{
    start_ms = millis();
    timer = lv_timer_create(reset_timer, 25, this);
    BasePage::show();
}

lv_obj_t *ResetPage::getPage()
{
    return page;
}

lv_obj_t *ResetPage::getResetMsgLabel()
{
    return reset_msg_label;
}

lv_timer_t *ResetPage::getTimer()
{
    return timer;
}

void ResetPage::setStartMs(unsigned long start_ms)
{
    this->start_ms = start_ms;
}
