
#pragma once
#include "./display/page_manager.h"
#include "./util.h"

class PageSelector : public BasePage
{
public:
    PageSelector(lv_obj_t *parent);
    void update(xSemaphoreHandle mutex, int16_t current_position) override;

private:
    lv_obj_t *selector_;

    lv_obj_t *hue_picker_label_;
    lv_obj_t *temp_picker_label_;
    lv_obj_t *presets_picker_label_;

    uint8_t pos_ = 0;
};
