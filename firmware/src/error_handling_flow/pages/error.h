#pragma once
#include "display/page_manager.h"

class ErrorPage : public BasePage
{
public:
    ErrorPage(lv_obj_t *parent);

    void setErrorTypeLabel(const char *label);
    void setErrorEventLabel(const char *label);

    lv_obj_t *error_label;
    lv_obj_t *error_event_label;
    lv_obj_t *error_type_label;
};