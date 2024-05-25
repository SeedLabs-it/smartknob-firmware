#include "light_switch.h"

LightSwitchApp::LightSwitchApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id) : App(mutex)
{
    sprintf(this->app_id, "%s", app_id);
    sprintf(this->friendly_name, "%s", friendly_name);
    sprintf(this->entity_id, "%s", entity_id);

    motor_config = PB_SmartKnobConfig{
        current_position,
        0,
        current_position,
        0,
        1,
        60 * PI / 180,
        1,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
        "",   // Change the type of app_id from char to char*
        0,
        {},
        0,
        27,
    };

    LV_IMAGE_DECLARE(x80_lightbulb_outline);
    LV_IMAGE_DECLARE(x40_lightbulb_outline);

    big_icon = x80_lightbulb_outline;
    small_icon = x40_lightbulb_outline;

    initScreen();
}

EntityStateUpdate LightSwitchApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;
    if (state_sent_from_hass)
    {
        state_sent_from_hass = false;
        return new_state;
    }

    current_position = state.current_position;
    sub_position_unit = state.sub_position_unit;
    // // needed to next reload of App
    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    adjusted_sub_position = sub_position_unit * motor_config.position_width_radians;
    // adjusted_sub_position = logf(1 - sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;

    if (current_position == 0 && adjusted_sub_position < 0)
    {
        adjusted_sub_position = 0;
    }
    else if (current_position == 1 && adjusted_sub_position > 0)
    {
        adjusted_sub_position = 0;
    }

    LOGE("LightSwitchApp::updateStateFromKnob: current_position: %d", current_position);
    LOGE("LightSwitchApp::updateStateFromKnob: sub_position_unit: %.2f", abs(adjusted_sub_position) * 100);

    if (abs(adjusted_sub_position) * 100 - abs(old_adjusted_sub_position) * 100 > 1)
    {
        SemaphoreGuard lock(mutex_);
        if (current_position == 0)
        {
            lv_arc_set_value(arc_, abs(adjusted_sub_position) * 100);
        }
        else
        {
            lv_arc_set_value(arc_, 100 - abs(adjusted_sub_position) * 100);
        }
    }

    if (last_position != current_position && first_run)
    {

        sprintf(new_state.app_id, "%s", app_id);
        sprintf(new_state.entity_id, "%s", entity_id);
        cJSON *json = cJSON_CreateObject();
        cJSON_AddBoolToObject(json, "on", current_position > 0);

        char *json_string = cJSON_PrintUnformatted(json);
        sprintf(new_state.state, "%s", json_string);

        cJSON_free(json_string);
        cJSON_Delete(json);

        last_position = current_position;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_LIGHT_SWITCH);
    }

    first_run = true;
    return new_state;
}

void LightSwitchApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
    cJSON *new_state = cJSON_Parse(mqtt_state_update.state);
    cJSON *on = cJSON_GetObjectItem(new_state, "on");

    if (on != NULL)
    {
        current_position = on->valueint;
        motor_config.position = current_position;
        motor_config.position_nonce = current_position + 1; // TODO: LOOK INTO THIS WEIRD WORK AROUND (NEEDED FOR LIGHT SWITCH NOT TO TOGGLE STATE OF LIGHT TO OFF IF SET TO ON IN HASS, KINDA)
        state_sent_from_hass = true;
    }

    cJSON_Delete(new_state);
}

void LightSwitchApp::updateStateFromSystem(AppState state) {}