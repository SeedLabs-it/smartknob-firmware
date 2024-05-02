#include "error_handling_flow.h"
// TODO Add reset ui flow/"hints".

ErrorHandlingFlow::ErrorHandlingFlow(TFT_eSprite *spr_, TFT_eSprite qrcode_spr_) : spr_(spr_), qrcode_spr_(qrcode_spr_)
{
}

void ErrorHandlingFlow::setQRCode(char *qr_data)
{
    QRCode qrcode;
    uint8_t qrcodeVersion = 6;
    int moduleSize = 2;

    uint8_t qrcodeData[qrcode_getBufferSize(qrcodeVersion)];
    qrcode_initText(&qrcode, qrcodeData, qrcodeVersion, 0, qr_data);

    int qrCodeWidthHeight = qrcode.size * moduleSize;
    qrcode_spr_.createSprite(qrCodeWidthHeight, qrCodeWidthHeight);
    qrcode_spr_.fillSprite(TFT_BLACK);

    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
            {
                qrcode_spr_.fillRect(x * moduleSize, y * moduleSize, moduleSize, moduleSize, TFT_WHITE);
            }
        }
    }
}

void ErrorHandlingFlow::handleEvent(WiFiEvent event)
{
    WiFiEvent send_event;
    motor_notifier->requestUpdate(blocked_motor_config);
    switch (event.type)
    {
    case SK_MQTT_RETRY_LIMIT_REACHED:
        if (!WiFi.isConnected())
        {
            sprintf(ip_data, "%s", "http://192.168.4.1/mqtt"); // always the same
        }
        else
        {
            // TODO: look into how to store and retrieve ip in a better way.
            sprintf(ip_data, "http://%s/mqtt", WiFi.localIP().toString().c_str());
        }
        setQRCode(ip_data);
    case SK_MQTT_CONNECTION_FAILED:
        error_type = MQTT_ERROR;
        latest_event = event;

        send_event.type = SK_MQTT_ERROR;
        publishEvent(send_event);
        break;
    case SK_WIFI_STA_RETRY_LIMIT_REACHED:
        if (!WiFi.isConnected())
        {
            sprintf(ip_data, "%s", "http://192.168.4.1/"); // always the same
        }
        else
        {
            // TODO: look into how to store and retrieve ip in a better way.
            sprintf(ip_data, "http://%s/", WiFi.localIP().toString().c_str());
        }
        setQRCode(ip_data);
    case SK_WIFI_STA_CONNECTION_FAILED:
        error_type = WIFI_ERROR;
        latest_event = event;

        send_event.type = SK_WIFI_ERROR;
        publishEvent(send_event);
        break;
    case SK_RESET_BUTTON_PRESSED:
        error_type = RESET;
        latest_event = event;
        break;
    case SK_RESET_BUTTON_RELEASED:
    case SK_DISMISS_ERROR:
    case SK_RESET_ERROR:
        error_type = NO_ERROR;
        break;
    default:
        break;
    }
}

void ErrorHandlingFlow::handleNavigationEvent(NavigationEvent event)
{
    WiFiEvent send_event;
    send_event.body.error.type = error_type;

    switch (event.press)
    {
    case NAVIGATION_EVENT_PRESS_SHORT:
        send_event.type = SK_RESET_ERROR;
        if (error_type == MQTT_ERROR && latest_event.type == SK_MQTT_RETRY_LIMIT_REACHED)
        {
            publishEvent(send_event);
        }
        else if (error_type == WIFI_ERROR && latest_event.type == SK_WIFI_STA_RETRY_LIMIT_REACHED)
        {
            publishEvent(send_event);
        }
        break;
    case NAVIGATION_EVENT_PRESS_LONG:
        send_event.type = SK_DISMISS_ERROR;
        if (error_type == MQTT_ERROR && latest_event.type == SK_MQTT_RETRY_LIMIT_REACHED)
        {
            publishEvent(send_event);
        }
        else if (error_type == WIFI_ERROR && latest_event.type == SK_WIFI_STA_RETRY_LIMIT_REACHED)
        {
            publishEvent(send_event);
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
            return renderConnectionFailed();
            break;
        case SK_MQTT_RETRY_LIMIT_REACHED:
            return renderRetryLimitReached();
            break;
        default:
            spr_->drawString("MQTT ERROR", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
            return spr_;
        }
        break;
    case WIFI_ERROR:
        switch (latest_event.type)
        {
        case SK_WIFI_STA_CONNECTION_FAILED:
            return renderConnectionFailed();
            break;
        case SK_WIFI_STA_RETRY_LIMIT_REACHED:
            return renderRetryLimitReached();
            break;
        default:
            spr_->drawString("WIFI ERROR", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
            return spr_;
        }
    case RESET:
        return renderResetInProgress();
        break;
    default:
        spr_->drawString("ERROR", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
        return spr_;
    }
    return spr_;
}

TFT_eSprite *ErrorHandlingFlow::renderResetInProgress()
{
    uint16_t center = TFT_WIDTH / 2;
    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setTextColor(TFT_BLACK);

    spr_->setFreeFont(&NDS1210pt7b);
    uint8_t held_for = (int)((millis() - latest_event.sent_at) / 1000);
    bool factory_reset = held_for > SOFT_RESET_SECONDS;

    spr_->fillScreen(factory_reset ? rgbToUint32(255, 0, 0) : rgbToUint32(255, 110, 0));

    sprintf(buf_, "%s", factory_reset ? "Factory reset" : "For soft reset");
    spr_->drawString(buf_, center, center - screen_name_label_h * 1.4, 1);

    sprintf(buf_, "%sin %ds", factory_reset ? "" : "release ", factory_reset ? max(0, HARD_RESET_SECONDS - held_for) : max(0, SOFT_RESET_SECONDS - held_for));
    spr_->drawString(buf_, center, center - screen_name_label_h * 0.4, 1);
    if (factory_reset)
    {
        spr_->drawString("Release for soft reset", center, center + screen_name_label_h, 1);
    }
    else
    {
        spr_->drawString("Release to cancel", center, center + screen_name_label_h, 1);
    }

    return this->spr_;
}

TFT_eSprite *ErrorHandlingFlow::renderConnectionFailed()
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

    switch (error_type)
    {
    case MQTT_ERROR:
        sprintf(buf_, "Retry %d", latest_event.body.error.body.mqtt_error.retry_count);

        break;
    case WIFI_ERROR:
        sprintf(buf_, "Retry %d", latest_event.body.error.body.wifi_error.retry_count);
        break;
    default:
        sprintf(buf_, "No retry count.");
        break;
    }

    spr_->drawString(buf_, center_horizontal, center_vertical - screen_name_label_h * 0.6, 1);

    spr_->setFreeFont(&NDS125_small);
    spr_->setTextColor(accent_text_color);

    sprintf(buf_, "Connection failed");
    spr_->drawString(buf_, center_horizontal, center_vertical + screen_name_label_h * 2, 1);

    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);

    switch (error_type)
    {
    case MQTT_ERROR:
        sprintf(buf_, "MQTT");
        break;
    case WIFI_ERROR:
        sprintf(buf_, "WIFI");
        break;
    default:
        sprintf(buf_, "ERROR");
        break;
    }
    spr_->drawString(buf_, center_horizontal, TFT_HEIGHT - screen_name_label_h * 2, 1);

    return this->spr_;
}

TFT_eSprite *ErrorHandlingFlow::renderRetryLimitReached()
{
    uint16_t center = TFT_WIDTH / 2;
    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setTextColor(accent_text_color);

    uint8_t qrsize = qrcode_spr_.width();
    qrcode_spr_.pushToSprite(spr_, center - qrsize / 2, center - qrsize / 2 - 6, TFT_BLACK);

    spr_->setFreeFont(&NDS125_small);
    spr_->drawString("Retry limit reached", center, center - screen_name_label_h * 3.4, 1);
    spr_->drawString("Press to retry", center, center - screen_name_label_h * 2.8, 1);
    spr_->drawString("Hold to dismiss", center, center + screen_name_label_h * 2, 1);

    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    switch (error_type)
    {
    case MQTT_ERROR:
        spr_->drawString("MQTT", center, TFT_HEIGHT - screen_name_label_h * 2, 1);
        break;
    case WIFI_ERROR:
        spr_->drawString("WIFI", center, TFT_HEIGHT - screen_name_label_h * 2, 1);
        break;
    default:
        spr_->drawString("ERROR", center, TFT_HEIGHT - screen_name_label_h * 2, 1);
        break;
    }

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
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue, &event, 0);
}

ErrorType ErrorHandlingFlow::getErrorType()
{
    return error_type;
}