#include "mqtt_error_flow.h"

MqttErrorFlow::MqttErrorFlow()
{
}

MqttErrorFlow::MqttErrorFlow(TFT_eSprite *spr)
{
    spr_ = spr;
}

void MqttErrorFlow::handleEvent(WiFiEvent event)
{
    latest_event = event;
    latest_event_received_at = millis();
    switch (event.type)
    {
    case MQTT_CONNECTION_FAILED:
        error_type = MQTT_ERROR;
        motor_notifier->requestUpdate(blocked_motor_config);
        break;
    case MQTT_RETRY_LIMIT_REACHED:
        error_type = MQTT_ERROR;
        motor_notifier->requestUpdate(blocked_motor_config);
        break;
    default:
        break;
    }
}

void MqttErrorFlow::handleNavigationEvent(NavigationEvent event)
{
    switch (event.press)
    {
    case NAVIGATION_EVENT_PRESS_SHORT:
        if (error_type == MQTT_ERROR && latest_event.type == MQTT_RETRY_LIMIT_REACHED)
        {
            WiFiEvent reset_error;
            reset_error.type = RESET_ERROR;
            reset_error.body.error.type = error_type;
            error_type = NO_ERROR;
            publishEvent(reset_error);
        }
        break;
    default:
        break;
    }
}

TFT_eSprite *MqttErrorFlow::render()
{
    switch (error_type)
    {
    case NO_ERROR:
        return spr_;
        break;
    case MQTT_ERROR:
        switch (latest_event.type)
        {
        case MQTT_CONNECTION_FAILED:
            return renderMqttConnectionFailed();
            break;
        case MQTT_RETRY_LIMIT_REACHED:
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

TFT_eSprite *MqttErrorFlow::renderMqttConnectionFailed()
{
    uint16_t center_vertical = TFT_HEIGHT / 2;
    uint16_t center_horizontal = TFT_WIDTH / 2;
    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);

    sprintf(buf_, "%ds", max(0, 10 - (int)((millis() - latest_event_received_at) / 1000))); // 10 should be same as wifi_client timeout in mqtt_task.cpp
    spr_->drawString(buf_, center_horizontal, screen_name_label_h, 1);

    sprintf(buf_, "Retry %d", latest_event.body.error.body.mqtt_error.retry_count);
    spr_->drawString(buf_, center_horizontal, screen_name_label_h * 2, 1);

    // spr_->drawString("CONTINUE IN", center_horizontal, center_vertical - screen_name_label_h, 1);
    spr_->drawString("Connection failed", center_horizontal, center_vertical + screen_name_label_h, 1);

    spr_->setTextColor(default_text_color);
    spr_->drawString("MQTT", center_horizontal, TFT_HEIGHT - screen_name_label_h * 2, 1);

    return this->spr_;
}

TFT_eSprite *MqttErrorFlow::renderMqttRetryLimitReached()
{
    uint16_t center_vertical = TFT_HEIGHT / 2;
    uint16_t center_horizontal = TFT_WIDTH / 2;
    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS1210pt7b);
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
        wifiqrcodestring = "http://10.0.0.1/mqtt"; // TODO: fetch real ip address
    }

    qrcode_initText(&qrcode, qrcodeData, qrcodeVersion, 0, wifiqrcodestring.c_str());

    int moduleSize = 2;

    int qrCodeWidth = qrcode.size * moduleSize;
    int qrCodeHeight = qrcode.size * moduleSize;

    int startX = center_horizontal - qrCodeWidth / 2;
    int startY = center_vertical - qrCodeHeight;

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
    spr_->drawString("Retry limit reached", center_horizontal, center_vertical + screen_name_label_h, 1);

    spr_->setTextColor(default_text_color);
    spr_->drawString("MQTT", center_horizontal, TFT_HEIGHT - screen_name_label_h * 2, 1);

    return this->spr_;
}

void MqttErrorFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
}

void MqttErrorFlow::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    this->shared_events_queue = shared_events_queue;
}

void MqttErrorFlow::publishEvent(WiFiEvent event)
{
    xQueueSendToBack(shared_events_queue, &event, 0);
}