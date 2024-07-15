#include "blinds.h"

BlindsApp::BlindsApp(SemaphoreHandle_t mutex, char *app_id_, char *friendly_name_, char *entity_id_) : App(mutex)
{
    sprintf(app_id, "%s", app_id_);
    sprintf(friendly_name, "%s", friendly_name_);
    sprintf(entity_id, "%s", entity_id_);

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
        "",
        0,
        {},
        0,
        27,
    };

    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    LV_IMG_DECLARE(x80_lightbulb_outline);
    LV_IMG_DECLARE(x40_lightbulb_outline);
    // LV_IMG_DECLARE(x80_lightbulb_filled);

    big_icon = x80_lightbulb_outline;
    // big_icon_active = x80_lightbulb_filled;
    small_icon = x40_lightbulb_outline;

    initScreen();
}

void BlindsApp::initScreen()
{
    SemaphoreGuard lock(mutex_);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, friendly_name);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    blinds_bar = lv_bar_create(screen);
    lv_obj_set_size(blinds_bar, 240, 260);
    lv_obj_set_style_bg_opa(blinds_bar, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_color(blinds_bar, LV_COLOR_MAKE(0xF8, 0xCA, 0x05), LV_PART_INDICATOR);
    lv_obj_set_style_radius(blinds_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(blinds_bar, 0, LV_PART_INDICATOR);
    lv_obj_center(blinds_bar);
    lv_bar_set_value(blinds_bar, 100, LV_ANIM_OFF);
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
            lv_bar_set_value(blinds_bar, (20 - current_closed_position) * 5, LV_ANIM_OFF);
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
}
void BlindsApp::updateStateFromSystem(AppState state)
{
}