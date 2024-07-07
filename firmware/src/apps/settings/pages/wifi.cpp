#include "wifi.h"

WiFiSettingsPage::WiFiSettingsPage(lv_obj_t *parent) : BasePage(parent)
{
    lv_obj_t *wifi_label = lv_label_create(page);
    lv_label_set_text(wifi_label, "WiFi");
    lv_obj_align(wifi_label, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t *container = lv_obj_create(page);
    lv_obj_set_size(container, 200, 200);
    lv_obj_center(container);

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t *wifi_quality_label = lv_label_create(container);
    lv_label_set_recolor(wifi_quality_label, true); /*Enable re-coloring by commands in the text*/
    lv_label_set_text(wifi_quality_label, "#00FF00 Excellent# test");
    lv_obj_set_style_text_align(wifi_quality_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_align(wifi_quality_label, LV_ALIGN_CENTER, 0);

    lv_obj_t *wifi_ssid_label = lv_label_create(container);
    lv_label_set_text(wifi_ssid_label, "SSID: ");
    lv_obj_set_style_text_align(wifi_ssid_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_align(wifi_ssid_label, LV_ALIGN_CENTER, 0);

    lv_obj_t *wifi_ip_label = lv_label_create(container);
    lv_label_set_text(wifi_ip_label, "IP: ");
    lv_obj_set_style_text_align(wifi_ip_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_align(wifi_ip_label, LV_ALIGN_CENTER, 0);

    // switch (connectivity_state.signal_strenth_status)
    // {
    // case 0:
    //     signal_strength_color = TFT_GREEN;
    //     signal_strength_text = "Excellent";
    //     break;
    // case 1:
    //     signal_strength_color = TFT_YELLOW;
    //     signal_strength_text = "Good";
    //     break;
    // case 2:
    //     signal_strength_color = TFT_ORANGE;
    //     signal_strength_text = "Fair";
    //     break;
    // case 3:
    //     signal_strength_color = TFT_RED;
    //     signal_strength_text = "Poor";
    //     break;
    // case 4:
    //     signal_strength_color = TFT_RED;
    //     signal_strength_text = "No signal";
    //     break;
    // default:
    //     break;
    // }
}