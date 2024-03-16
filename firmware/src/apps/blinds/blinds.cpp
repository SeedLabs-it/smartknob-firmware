#include "blinds.h"

BlindsApp::BlindsApp(TFT_eSprite *spr_, char *app_id, char *friendly_name) : App(spr_)
{
    this->app_id = app_id;
    this->friendly_name = friendly_name;

    motor_config = PB_SmartKnobConfig{
        15,
        0,
        15,
        0,
        20,
        8.225806452 * PI / 120,
        2,
        1,
        1.1,
        "SKDEMO_Shades",
        0,
        {},
        0,
        27,
    };

    big_icon = shades_80;
    small_icon = shades_40;
}

int8_t BlindsApp::navigationNext()
{
    motor_config.position_nonce = motor_config.position;
    motor_notifier->requestUpdate(motor_config);

    if (motor_config.position == 0)
    {
        motor_config.position = 20;
    }
    else if (motor_config.position > 0)
    {
        motor_config.position = 0;
    }

    motor_config.position_nonce = motor_config.position;

    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}

void BlindsApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
    cJSON *position = cJSON_GetObjectItem(mqtt_state_update.state, "position");

    if (position != NULL)
    {
        current_closed_position = (20 - position->valueint / 5);
        motor_config.position = current_closed_position;
        motor_config.position_nonce = current_closed_position;
        state_sent_from_hass = true;
    }
}

EntityStateUpdate BlindsApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;
    if (state_sent_from_hass)
    {
        state_sent_from_hass = false;
        return new_state;
    }

    current_closed_position = state.current_position;

    motor_config.position_nonce = current_closed_position;
    motor_config.position = current_closed_position;

    if (last_closed_position != current_closed_position && !state_sent_from_hass)
    {
        sprintf(new_state.app_id, "%s", app_id);

        cJSON *json = cJSON_CreateObject();

        cJSON_AddNumberToObject(json, "position", (20 - current_closed_position) * 5);

        char *json_str = cJSON_PrintUnformatted(json);
        sprintf(new_state.state, "%s", json_str);

        cJSON_free(json_str);
        cJSON_Delete(json);

        last_closed_position = current_closed_position;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_BLINDS);
    }

    return new_state;
}

void BlindsApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *BlindsApp::render()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    uint32_t light_background = spr_->color565(150, 150, 150);

    uint32_t shade_bar_color = TFT_OLIVE;
    uint32_t shade_color = spr_->color565(50, 50, 50);

    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, light_background);
    int32_t height = (current_closed_position - motor_config.min_position) * TFT_HEIGHT / (motor_config.max_position - motor_config.min_position);
    spr_->fillRect(0, 0, TFT_WIDTH, height, shade_color);
    spr_->fillRect(0, height - 10, TFT_WIDTH, 10, shade_bar_color);

    spr_->setFreeFont(&Roboto_Light_60);

    uint16_t center = TFT_WIDTH / 2;

    uint8_t arrow_size = 50;

    if (current_closed_position == 0)
    {
        sprintf(buf_, "%s", "Opened");
        spr_->drawBitmap(center - arrow_size / 2, TFT_HEIGHT - 20 - arrow_size, arrow_down_50, arrow_size, arrow_size, shade_bar_color, light_background);
    }
    else if (current_closed_position == 10)
    {
        sprintf(buf_, "%s", "Half");
    }
    else if (current_closed_position == 20)
    {
        sprintf(buf_, "%s", "Closed");
        spr_->drawBitmap(center - arrow_size / 2, TFT_HEIGHT - 20 - arrow_size, arrow_up_50, arrow_size, arrow_size, shade_bar_color, shade_color);
    }
    else
    {
        sprintf(buf_, "%d%%", current_closed_position * 5);
        // spr_->drawBitmap(center - arrow_size / 2, TFT_HEIGHT - 20 - arrow_size, arrow_up_50, arrow_size, arrow_size, TFT_OLIVE, TFT_TRANSPARENT);
    }

    spr_->drawString(buf_, TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);

    // spr_->drawNumber(current_closed_position, TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Thin_24);
    spr_->drawString("Bedroom shade", TFT_WIDTH / 2, TFT_HEIGHT / 2 - 20 - 30, 1);

    return this->spr_;
};