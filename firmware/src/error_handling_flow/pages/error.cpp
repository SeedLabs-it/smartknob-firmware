#include "./error.h"

void retry_timer(lv_timer_t *timer)
{
    ErrorPage *error_page = (ErrorPage *)timer->user_data;

    uint8_t seconds_since_event = (int)((millis() - error_page->error_state.latest_event.sent_at) / 1000);
    lv_label_set_text_fmt(error_page->countdown_label, "%ds", max(0, 10 - seconds_since_event));
    lv_obj_align(error_page->countdown_label, LV_ALIGN_CENTER, 0, -20);

    uint8_t retries = error_page->error_state.retry_count;
    lv_label_set_text_fmt(error_page->retry_label, "Retry %d", retries);
    lv_obj_align_to(error_page->retry_label, error_page->countdown_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

ErrorPage::ErrorPage(lv_obj_t *parent) : BasePage(parent)
{
    error_label = lv_label_create(page);
    lv_label_set_text(error_label, "ERROR");
    lv_obj_set_style_text_color(error_label, LV_COLOR_MAKE(0xED, 0x43, 0x37), LV_PART_MAIN);
    lv_obj_align(error_label, LV_ALIGN_TOP_MID, 0, 10);

    error_type_label = lv_label_create(page);
    lv_obj_align(error_type_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_text_font(error_type_label, &aktivgrotesk_regular_12pt, 0);

    error_event_label = lv_label_create(page);
    lv_obj_set_style_text_color(error_event_label, LV_COLOR_MAKE(0xED, 0x43, 0x37), LV_PART_MAIN);
    lv_obj_align_to(error_event_label, error_type_label, LV_ALIGN_OUT_TOP_MID, 0, -10);

    countdown_label = lv_label_create(page);
    lv_label_set_text(countdown_label, "10s");
    lv_obj_align(countdown_label, LV_ALIGN_CENTER, 0, -20);

    retry_label = lv_label_create(page);
    lv_label_set_text(retry_label, "Retry 1");
    lv_obj_align_to(retry_label, countdown_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    qr_code = lv_qrcode_create(page, 80, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_COLOR_MAKE(0x00, 0x00, 0x00));
    lv_qrcode_update(qr_code, "placeholder", strlen("placeholder"));
    lv_obj_align_to(qr_code, error_event_label, LV_ALIGN_OUT_TOP_MID, 0, -10);

    press_to_retry_label = lv_label_create(page);
    lv_label_set_text(press_to_retry_label, "Press to retry\nor hold to dismiss");
    lv_obj_set_style_text_align(press_to_retry_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(press_to_retry_label, qr_code, LV_ALIGN_OUT_TOP_MID, 0, -10);

    lv_obj_add_flag(qr_code, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(press_to_retry_label, LV_OBJ_FLAG_HIDDEN);
}

void ErrorPage::show()
{
    LOGE("ErrorPage::show()");
    LOGE("Error type: %d", error_state.latest_error_type);
    LOGE("Error event: %d", error_state.latest_event.type);
    switch (error_state.latest_error_type)
    {
    case ErrorType::MQTT_ERROR:
        lv_label_set_text(error_type_label, "MQTT");
        break;
    case ErrorType::WIFI_ERROR:
        lv_label_set_text(error_type_label, "WIFI");
        break;
    default:
        break;
    }

    switch (error_state.latest_event.type)
    {
    case EventType::SK_MQTT_CONNECTION_FAILED:
    case EventType::SK_WIFI_STA_CONNECTION_FAILED:
        lv_obj_add_flag(qr_code, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(press_to_retry_label, LV_OBJ_FLAG_HIDDEN);

        lv_label_set_text(error_event_label, "Connection failed");
        if (timer == nullptr)
        {
            timer = lv_timer_create(retry_timer, 250, this);
        }
        break;
    case EventType::SK_MQTT_RETRY_LIMIT_REACHED:
    case EventType::SK_WIFI_STA_RETRY_LIMIT_REACHED:
        lv_timer_del(timer);
        timer = nullptr;
        lv_label_set_text(countdown_label, "");
        lv_label_set_text(retry_label, "");

        lv_obj_clear_flag(qr_code, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(press_to_retry_label, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(error_event_label, "Retry limit reached");
        break;

    default:
        break;
    }

    lv_obj_align(error_type_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_align_to(error_event_label, error_type_label, LV_ALIGN_OUT_TOP_MID, 0, -10);

    BasePage::show();
}

void ErrorPage::setQr(const char *qr_data)
{
    lv_qrcode_update(qr_code, qr_data, strlen(qr_data));
}

lv_obj_t *ErrorPage::getPage()
{
    return page;
}
