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
        error_mode = MQTT_ERROR;
        motor_notifier->requestUpdate(blocked_motor_config);
        break;
    case MQTT_RETRY_LIMIT_REACHED:
        error_mode = MQTT_ERROR;
        motor_notifier->requestUpdate(blocked_motor_config);
        break;
    default:
        break;
    }
}

void MqttErrorFlow::handleNavigationEvent(NavigationEvent event)
{
    // switch (event.press)
    // {
    // default:
    //     motor_notifier->requestUpdate(blocked_motor_config);
    //     break;
    // }
}

TFT_eSprite *MqttErrorFlow::render()
{
    switch (error_mode)
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

    sprintf(buf_, "Retry %d", latest_event.body.mqtt_error.connection_retry_count);
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

    // spr_->drawString("MQTT", center_horizontal, center_vertical - screen_name_label_h, 1);

    spr_->drawString("Retry limit reached", center_horizontal, center_vertical + screen_name_label_h, 1);

    spr_->setTextColor(default_text_color);
    spr_->drawString("MQTT", center_horizontal, TFT_HEIGHT - screen_name_label_h * 2, 1);

    return this->spr_;
}

void MqttErrorFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
}