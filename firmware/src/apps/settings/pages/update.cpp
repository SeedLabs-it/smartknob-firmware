#include "update.h"

UpdateSettingsPage::UpdateSettingsPage(lv_obj_t *parent) : BasePage(parent)
{
    // static lv_style_t *text_style = new lv_style_t;
    // lv_style_init(text_style);

    lv_obj_t *container = lv_obj_create(page);
    lv_obj_set_style_bg_opa(container, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(container, LV_OPA_0, 0);
    lv_obj_center(container);

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(container, 6, 0);

    // lv_obj_add_style(container, text_style, 0);
    update_label = lv_label_create(container);
    lv_obj_set_style_text_font(update_label, &aktivgrotesk_regular_12pt_8bpp_subpixel, LV_PART_MAIN);
    lv_label_set_text(update_label, "SCAN TO UPDATE");

    update_qrcode = lv_qrcode_create(container, 80, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));
    lv_qrcode_update(update_qrcode, "http://192.168.4.1/update", strlen("http://192.168.4.1/update"));
    lv_obj_align(update_qrcode, LV_ALIGN_CENTER, 0, -20);

    update_url_label = lv_label_create(container);
    lv_obj_set_style_text_font(update_url_label, &aktivgrotesk_regular_12pt_8bpp_subpixel, LV_PART_MAIN);
    lv_label_set_text(update_url_label, "192.168.4.1/update");
}

void UpdateSettingsPage::updateFromSystem(AppState state)
{
    if (state.connectivity_state.is_connected != last_connectivity_state.is_connected)
    {
        lv_label_set_text_fmt(update_label, "SCAN TO UPDATE");

        if (!state.connectivity_state.is_connected)
        {
            lv_qrcode_update(update_qrcode, "http://192.168.4.1/update", strlen("http://192.168.4.1/update"));
        }
        else
        {
            char ip_data[50];
            sprintf(ip_data, "http://%s/update", state.connectivity_state.ip_address);
            lv_qrcode_update(update_qrcode, ip_data, strlen(ip_data));
            lv_label_set_text_fmt(update_url_label, "%s", ip_data);
        }

        last_connectivity_state = state.connectivity_state;
    }
}