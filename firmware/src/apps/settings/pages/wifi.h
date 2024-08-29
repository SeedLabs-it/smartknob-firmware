#pragma once
#include "./display/page_manager.h"

class WiFiSettingsPage : public BasePage
{
public:
    WiFiSettingsPage(lv_obj_t *parent);
    void updateFromSystem(AppState state) override;

private:
    lv_obj_t *wifi_quality_label;
    lv_obj_t *wifi_ssid_label;
    lv_obj_t *wifi_ip_label;

    const char *getSignalStrengthTextWithStatus(uint8_t signal_strength_status);
    char signal_strength_text_[32];

    ConnectivityState state_;
};