#include "blinds.h"

BlindsApp::BlindsApp(SemaphoreHandle_t mutex, char *app_id_, char *friendly_name_, char *entity_id_) : App(mutex)
{
    sprintf(app_id, "%s", app_id_);
    sprintf(friendly_name, "%s", friendly_name_);
    sprintf(entity_id, "%s", entity_id_);

    motor_config = PB_SmartKnobConfig{
        current_closed_position,
        0,
        15,
        0,
        20,
        8.225806452 * PI / 120,
        2,
        1,
        1.1,
        "",
        0,
        {},
        0,
        27,
    };

    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    LV_IMG_DECLARE(x80_blind);
    LV_IMG_DECLARE(x40_blind);

    big_icon = x80_blind;
    small_icon = x40_blind;

    initScreen();
}

void BlindsApp::initScreen()
{
    SemaphoreGuard lock(mutex_);

    blinds_bar = lv_bar_create(screen);
    lv_obj_set_size(blinds_bar, 240, 242); //-Border width
    lv_obj_set_style_bg_opa(blinds_bar, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_color(blinds_bar, LV_COLOR_MAKE(0xCC, 0xCC, 0xCC), LV_PART_INDICATOR);
    lv_obj_set_style_radius(blinds_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(blinds_bar, 0, LV_PART_INDICATOR);
    lv_obj_center(blinds_bar);
    lv_bar_set_value(blinds_bar, 100, LV_ANIM_OFF);

    lv_obj_set_style_border_side(blinds_bar, LV_BORDER_SIDE_TOP, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(blinds_bar, 8, LV_PART_INDICATOR);
    lv_obj_set_style_border_color(blinds_bar, LV_COLOR_MAKE(0xF8, 0xCA, 0x05), LV_PART_INDICATOR);

    lv_obj_t *friendly_name_label = lv_label_create(screen);
    lv_obj_set_style_text_font(friendly_name_label, &nds12_14px, 0);
    lv_label_set_text(friendly_name_label, friendly_name);
    lv_obj_align(friendly_name_label, LV_ALIGN_CENTER, 0, -30);

    percentage_label = lv_label_create(screen);
    lv_obj_set_style_text_font(percentage_label, &nds12_20px, 0);
    lv_label_set_text(percentage_label, "Open");
    lv_obj_align(percentage_label, LV_ALIGN_CENTER, 0, 0);
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
        {
            SemaphoreGuard lock(mutex_);
            uint8_t percentage = (20 - current_closed_position) * 5;
            lv_bar_set_value(blinds_bar, (20 - current_closed_position) * 5, LV_ANIM_OFF);

            if (current_closed_position == 0)
            {
                lv_label_set_text(percentage_label, "Open");
            }
            else if (current_closed_position == 20)
            {
                lv_label_set_text(percentage_label, "Closed");
            }
            else if (current_closed_position > 0 && current_closed_position < 20)
            {
                lv_label_set_text_fmt(percentage_label, "%d%%", percentage);
            }
            lv_obj_align(percentage_label, LV_ALIGN_CENTER, 0, 0);
        }

        sprintf(new_state.app_id, "%s", app_id);
        sprintf(new_state.entity_id, "%s", entity_id);

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

void BlindsApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
    cJSON *new_state = cJSON_Parse(mqtt_state_update.state);
    cJSON *position = cJSON_GetObjectItem(new_state, "position");

    if (position != NULL)
    {
        current_closed_position = (20 - position->valueint / 5);
        motor_config.position = current_closed_position;
        motor_config.position_nonce = current_closed_position;
        state_sent_from_hass = true;
    }

    cJSON_Delete(new_state);
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