
#pragma once
#include "./display/page_manager.h"

class UpdateSettingsPage : public BasePage
{
public:
    UpdateSettingsPage(lv_obj_t *parent);
    void updateFromSystem(AppState state) override;

private:
    lv_obj_t *update_label;
    lv_obj_t *update_qrcode;
    lv_obj_t *update_url_label;

    ConnectivityState last_connectivity_state;
};
