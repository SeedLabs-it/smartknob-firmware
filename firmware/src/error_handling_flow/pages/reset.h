#pragma once
#include "display/page_manager.h"

class ResetPage : public BasePage
{
public:
    ResetPage(lv_obj_t *parent);

    void render();

    lv_obj_t *getParent();

    lv_obj_t *getResetMsgLabel();
    lv_timer_t *getTimer();

    void setStartMs(unsigned long start_ms);

    lv_obj_t *reset_label;
    lv_obj_t *reset_msg_label;

    unsigned long start_ms;

    lv_timer_t *timer;
};