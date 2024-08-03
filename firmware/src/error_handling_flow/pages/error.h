#pragma once
#include "display/page_manager.h"

class ErrorPage : public BasePage
{
public:
    ErrorPage(lv_obj_t *parent);

    void show();

    void setErrorTypeLabel(const char *label);
    void setErrorEventLabel(const char *label);

    void setQr(const char *qr_data);

    lv_obj_t *getPage();

    lv_obj_t *error_label;
    lv_obj_t *error_event_label;
    lv_obj_t *error_type_label;

    lv_obj_t *countdown_label;
    lv_obj_t *retry_label;

    lv_obj_t *qr_code;
    lv_obj_t *press_to_retry_label;

    ErrorState error_state;
    lv_timer_t *timer = nullptr;
};