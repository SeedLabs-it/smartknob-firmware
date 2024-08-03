#include "reset.h"

void reset_timer(lv_timer_t *timer)
{
    // LOGE("reset_timer");
    /*Use the user_data*/
    ResetPage *reset_page = (ResetPage *)timer->user_data;
    // unsigned long now = millis();
    // unsigned long diff_ms = now - user_data->start_ms;
    // unsigned long stopwatch_ms = 0;
    // unsigned long stopwatch_sec = 0;
    // unsigned long stopwatch_min = 0;

    // stopwatch_ms = diff_ms % 100;
    // stopwatch_sec = floor((diff_ms / 1000) % 60);
    // stopwatch_min = floor((diff_ms / (1000 * 60)) % 60);

    // lv_label_set_text_fmt(user_data->time_label, "%02d:%02d.", stopwatch_min, stopwatch_sec);
    // lv_label_set_text_fmt(user_data->ms_label, "%02d", stopwatch_ms);

    lv_obj_t *reset_msg_label = reset_page->getResetMsgLabel();
    lv_obj_t *parent = reset_page->getParent();

    uint8_t held_for = (int)((millis() - reset_page->start_ms) / 1000);
    if (held_for > SOFT_RESET_SECONDS)
    {
        lv_obj_set_style_bg_color(parent, LV_COLOR_MAKE(0xFF, 0x00, 0x00), LV_PART_MAIN);
        lv_label_set_text_fmt(reset_msg_label, "FOR FACTORY RESET\nRELEASE IN %ds \nRELEASE FOR SOFT RESET", max(0, HARD_RESET_SECONDS - held_for));
        // reset_page->setResetTypeLabel("FOR FACTORY RESET\nRELEASE IN 5s \nRELEASE FOR SOFT_RESET");
    }
    else
    {
        lv_obj_set_style_bg_color(parent, LV_COLOR_MAKE(0xFF, 0x6E, 0x00), LV_PART_MAIN);
        lv_label_set_text_fmt(reset_msg_label, "FOR SOFT RESET\nRELEASE IN %ds \nRELEASE TO CANCEL", max(0, SOFT_RESET_SECONDS - held_for));
        lv_obj_align(reset_msg_label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);

        // reset_page->setResetTypeLabel("FOR SOFT RESET\nRELEASE IN 5s \nRELEASE TO CANCEL");
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
}

void ResetPage::render()
{
    start_ms = millis();
    timer = lv_timer_create(reset_timer, 25, this);
    BasePage::render();
}

lv_obj_t *ResetPage::getParent()
{
    return parent;
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
