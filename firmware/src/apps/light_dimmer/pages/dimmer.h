#pragma once
#include "./display/page_manager.h"
#include "./util.h"

class DimmerPage : public BasePage
{
public:
    DimmerPage(lv_obj_t *parent, const AppData &app_data);
    void update(xSemaphoreHandle mutex, int16_t position) override;
    void handleNavigation(NavigationEvent event) override;

    void updateArcColor(lv_color_t color);

private:
    lv_obj_t *arc_;
    lv_obj_t *percentage_label_;
    lv_obj_t *friendly_name_label_;

    int16_t current_position_ = 0;
};