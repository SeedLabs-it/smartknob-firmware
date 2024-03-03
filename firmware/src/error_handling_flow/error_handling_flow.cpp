#include "error_handling_flow.h"

ErrorHandlingFlow::ErrorHandlingFlow()
{
}

ErrorHandlingFlow::ErrorHandlingFlow(TFT_eSprite *spr)
{
    spr_ = spr;
}

void ErrorHandlingFlow::handleEvent(WiFiEvent event)
{
    motor_notifier->requestUpdate(blocked_motor_config);
    latest_event = event;
    switch (event.type)
    {
    case SK_MQTT_CONNECTION_FAILED:
    case SK_MQTT_RETRY_LIMIT_REACHED:
        error_type = MQTT_ERROR;
        break;
    case SK_WIFI_STA_CONNECTION_FAILED:
    case SK_WIFI_STA_RETRY_LIMIT_REACHED:
        error_type = WIFI_ERROR;
        break;
    default:
        break;
    }
}

void ErrorHandlingFlow::handleNavigationEvent(NavigationEvent event)
{
    switch (event.press)
    {
    case NAVIGATION_EVENT_PRESS_SHORT:
        if (error_type == MQTT_ERROR && latest_event.type == SK_MQTT_RETRY_LIMIT_REACHED)
        {
            WiFiEvent reset_error;
            reset_error.type = SK_RESET_ERROR;
            reset_error.body.error.type = error_type;
            error_type = NO_ERROR;
            publishEvent(reset_error);
        }
        break;
    default:
        break;
    }
}

TFT_eSprite *ErrorHandlingFlow::render()
{
    switch (error_type)
    {
    case NO_ERROR:
        return spr_;
        break;
    case MQTT_ERROR:
        switch (latest_event.type)
        {
        case SK_MQTT_CONNECTION_FAILED:
            return renderMqttConnectionFailed();
            break;
        case SK_MQTT_RETRY_LIMIT_REACHED:
            return renderMqttRetryLimitReached();
            break;
        default:
            return spr_;
        }
        break;
    default:
        return spr_;
    }
}

TFT_eSprite *ErrorHandlingFlow::renderMqttConnectionFailed()
{
    uint16_t center_vertical = TFT_HEIGHT / 2;
    uint16_t center_horizontal = TFT_WIDTH / 2;
    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);

    sprintf(buf_, "%ds", max(0, 10 - (int)((millis() - latest_event.sent_at) / 1000))); // 10 should be same as wifi_client timeout in mqtt_task.cpp
    spr_->drawString(buf_, center_horizontal, center_vertical - screen_name_label_h * 1.6, 1);

    sprintf(buf_, "Retry %d", latest_event.body.error.body.mqtt_error.retry_count);
    spr_->drawString(buf_, center_horizontal, center_vertical - screen_name_label_h * 0.6, 1);

    // spr_->drawString("CONTINUE IN", center_horizontal, center_vertical - screen_name_label_h, 1);
    spr_->setFreeFont(&NDS125_small);
    spr_->setTextColor(accent_text_color);
    spr_->drawString("Connection failed", center_horizontal, center_vertical + screen_name_label_h * 2, 1);

    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString("MQTT", center_horizontal, TFT_HEIGHT - screen_name_label_h * 2, 1);

    return this->spr_;
}

TFT_eSprite *ErrorHandlingFlow::renderMqttRetryLimitReached()
{
    uint16_t center_vertical = TFT_HEIGHT / 2;
    uint16_t center_horizontal = TFT_WIDTH / 2;
    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    // spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);

    QRCode qrcode;
    uint8_t qrcodeVersion = 6;
    uint8_t qrcodeData[qrcode_getBufferSize(qrcodeVersion)];
    // ESP_LOGD("HassSetupApp", "AP IP: %s", state.connectivity_state.ap_ip_address.toString().c_str());
    std::string wifiqrcodestring = "";
    if (!WiFi.isConnected())
    {
        wifiqrcodestring = "http://192.168.4.1/mqtt"; // always the same
    }
    else
    {
        sprintf(buf_, "http://%s/mqtt", WiFi.localIP().toString().c_str());
        wifiqrcodestring = buf_;
    }

    qrcode_initText(&qrcode, qrcodeData, qrcodeVersion, 0, wifiqrcodestring.c_str());

    int moduleSize = 2;

    int qrCodeWidth = qrcode.size * moduleSize;
    int qrCodeHeight = qrcode.size * moduleSize;

    int startX = center_horizontal - qrCodeWidth / 2;
    int startY = center_vertical - qrCodeHeight / 1.4;

    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
            {
                spr_->fillRect(startX + x * moduleSize, startY + y * moduleSize, moduleSize, moduleSize, TFT_WHITE);
            }
        }
    }
    spr_->setFreeFont(&NDS125_small);
    spr_->drawString("Retry limit reached", center_horizontal, center_vertical + screen_name_label_h * 2, 1);

    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString("MQTT", center_horizontal, TFT_HEIGHT - screen_name_label_h * 2, 1);

    return this->spr_;
}

void ErrorHandlingFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
}

void ErrorHandlingFlow::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    this->shared_events_queue = shared_events_queue;
}

void ErrorHandlingFlow::publishEvent(WiFiEvent event)
{
    xQueueSendToBack(shared_events_queue, &event, 0);
}