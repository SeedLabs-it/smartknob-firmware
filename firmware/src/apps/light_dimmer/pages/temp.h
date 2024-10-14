
#pragma once
#include "./display/page_manager.h"
#include "./util.h"

class TempPage : public BasePage
{
public:
    TempPage(lv_obj_t *parent);
    void updateFromSystem(AppState state) override;
    void handleNavigation(NavigationEvent event) override;

private:
    lv_obj_t *temp_wheel;
    lv_obj_t *temp_selector;
    lv_obj_t *temp_label;
};
