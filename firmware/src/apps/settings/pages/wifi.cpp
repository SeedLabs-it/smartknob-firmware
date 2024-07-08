#include "wifi.h"

WiFiSettingsPage::WiFiSettingsPage(lv_obj_t *parent) : BasePage(parent)
{
    static lv_style_t *text_style = new lv_style_t;
    lv_style_init(text_style);
    lv_style_set_text_font(text_style, &NDS12_20px);

    lv_obj_t *container = lv_obj_create(page);
    lv_obj_set_style_bg_opa(container, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(container, LV_OPA_0, 0);
    lv_obj_center(container);

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(container, 6, 0);

    lv_obj_add_style(container, text_style, 0);

    wifi_quality_label = lv_label_create(container);
    lv_label_set_recolor(wifi_quality_label, true); /*Enable re-coloring by commands in the text*/
    lv_label_set_text(wifi_quality_label, "");

    wifi_ssid_label = lv_label_create(container);
    lv_label_set_text(wifi_ssid_label, "");

    wifi_ip_label = lv_label_create(container);
    lv_label_set_text(wifi_ip_label, "");
}

const char *getSignalStrengthTextWithStatus(uint8_t signal_strength_status)
{
    switch (signal_strength_status)
    {
    case 0:
        return "Excellent";
    case 1:
        return "Good";
    case 2:
        return "Fair";
    case 3:
        return "Poor";
    case 4:
        return "No signal";
    default:
        return "Unknown";
    }
}

void WiFiSettingsPage::updateFromSystem(AppState state)
{
    if (state.connectivity_state.is_connected)
    {
        lv_label_set_text_fmt(wifi_quality_label, "#00FF00 %s# %d", getSignalStrengthTextWithStatus(state.connectivity_state.signal_strenth_status), state.connectivity_state.signal_strength); //! Add different colors for different signal strengths
    }
    else
    {
        lv_label_set_text_fmt(wifi_quality_label, "#FFA500 Disconnected#");
    }

    if (strcmp(state.connectivity_state.ip_address, "") != 0)
    {
        lv_label_set_text_fmt(wifi_ip_label, "IP: %s", state.connectivity_state.ip_address);
    }
    else
    {
        lv_label_set_text_fmt(wifi_ip_label, "IP: %s", "N/A");
    }

    if (strcmp(state.connectivity_state.ssid, "") != 0)
    {
        lv_label_set_text_fmt(wifi_ssid_label, "SSID: %s", state.connectivity_state.ssid);
    }
    else
    {
        lv_label_set_text_fmt(wifi_ssid_label, "SSID: %s", "N/A");
    }
}