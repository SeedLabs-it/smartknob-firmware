#include "switch.h"

SwitchApp::SwitchApp(SemaphoreHandle_t mutex, char *app_id_, char *friendly_name_, char *entity_id_, bool is_light_switch_) : App(mutex), is_light_switch(is_light_switch_)
{
    sprintf(app_id, "%s", app_id_);
    sprintf(friendly_name, "%s", friendly_name_);
    sprintf(entity_id, "%s", entity_id_);

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
        "",
        0,
        {},
        0,
        27,
    };
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    if (is_light_switch)
    {
        LV_IMG_DECLARE(x80_lightbulb_outline);
        LV_IMG_DECLARE(x40_lightbulb_outline);
        LV_IMG_DECLARE(x80_lightbulb_filled);

        big_icon = x80_lightbulb_outline;
        big_icon_active = x80_lightbulb_filled;
        small_icon = x40_lightbulb_outline;
    }
    else
    {
        LV_IMG_DECLARE(x80_toggle_switch_off);
        LV_IMG_DECLARE(x40_toggle_switch_off);
        LV_IMG_DECLARE(x80_toggle_switch_on);

        big_icon = x80_toggle_switch_off;
        big_icon_active = x80_toggle_switch_on;
        small_icon = x40_toggle_switch_off;
    }

    initScreen();
}

void SwitchApp::initScreen()
{
    SemaphoreGuard lock(mutex_);

    arc_ = lv_arc_create(screen);
    lv_obj_set_size(arc_, 210, 210);
    lv_arc_set_rotation(arc_, 225);
    lv_arc_set_bg_angles(arc_, 0, 90);
    lv_arc_set_value(arc_, 0);
    lv_obj_center(arc_);

    lv_obj_set_style_arc_opa(arc_, LV_OPA_0, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
    lv_obj_set_style_bg_color(arc_, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_KNOB);

    lv_obj_set_style_arc_width(arc_, 24, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, 24, LV_PART_INDICATOR);
    lv_obj_set_style_pad_all(arc_, -5, LV_PART_KNOB);

    if (is_light_switch)
    {
        status_label = lv_img_create(screen);
        lv_img_set_src(status_label, &big_icon);
        lv_obj_set_style_img_recolor_opa(status_label, LV_OPA_COVER, 0);
        lv_obj_set_style_img_recolor(status_label, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), 0);
    }
    else
    {
        status_label = lv_label_create(screen);
        lv_label_set_text(status_label, "OFF");
        lv_obj_set_style_text_color(status_label, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), 0);
    }
    lv_obj_center(status_label);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, friendly_name);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -48);
}
float previous_sub_position_unit = 0.0f;

EntityStateUpdate SwitchApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;
    if (state_sent_from_hass)
    {
        state_sent_from_hass = false;
        return new_state;
    }

    current_position = state.current_position;
    sub_position_unit = state.sub_position_unit * motor_config.position_width_radians;
    // needed to next reload of App
    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    float vel = (sub_position_unit * 100 - previous_sub_position_unit * 100) / (millis() - last_updated_ms);

    if (abs(vel) > 0.75f || current_position != last_position)
    {
        if (current_position == 0 && sub_position_unit < 0)
        {
            sub_position_unit = 0;
        }
        else if (current_position == 1 && sub_position_unit > 0)
        {
            sub_position_unit = 0;
        }

        SemaphoreGuard lock(mutex_);
        if (current_position == 0)
        {
            lv_arc_set_value(arc_, abs(sub_position_unit) * 100);
        }
        else
        {
            lv_arc_set_value(arc_, 100 - abs(sub_position_unit) * 100);
        }
    }
    else
    {
        if (current_position == 0)
        {
            lv_arc_set_value(arc_, 0);
        }
        else
        {
            lv_arc_set_value(arc_, 100);
        }
    }

    if (last_position != current_position && first_run)
    {
        {
            SemaphoreGuard lock(mutex_);
            if (current_position == 0)
            {
                if (is_light_switch)
                {
                    lv_img_set_src(status_label, &big_icon);
                }
                else
                {
                    lv_label_set_text(status_label, "OFF");
                }
                lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
                lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
            }
            else
            {
                if (is_light_switch)
                {
                    lv_img_set_src(status_label, &big_icon_active);
                    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0xFF, 0x9E, 0x00), 0);
                    lv_obj_set_style_arc_color(arc_, lv_color_mix(dark_arc_bg, LV_COLOR_MAKE(0xFF, 0x9E, 0x00), 128), LV_PART_MAIN);
                }
                else
                {
                    lv_label_set_text(status_label, "ON");
                    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x80, 0x00), 0);
                    lv_obj_set_style_arc_color(arc_, lv_color_mix(dark_arc_bg, LV_COLOR_MAKE(0x00, 0x80, 0x00), 128), LV_PART_MAIN);
                }
            }
        }
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

        if (is_light_switch)
        {
            sprintf(new_state.app_slug, "%s", APP_SLUG_LIGHT_SWITCH);
        }
        else
        {
            sprintf(new_state.app_slug, "%s", APP_SLUG_SWITCH);
        }
    }

    last_updated_ms = millis();

    first_run = true;
    return new_state;
}

void SwitchApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
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

    last_position = current_position;

    {
        SemaphoreGuard lock(mutex_);

        if (current_position == 0)
        {
            if (is_light_switch)
            {
                lv_img_set_src(status_label, &big_icon);
            }
            else
            {
                lv_label_set_text(status_label, "OFF");
            }
            lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
            lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
        }
        else
        {
            if (is_light_switch)
            {
                lv_img_set_src(status_label, &big_icon_active);
                lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0xFF, 0x9E, 0x00), 0);
                lv_obj_set_style_arc_color(arc_, lv_color_mix(dark_arc_bg, LV_COLOR_MAKE(0xFF, 0x9E, 0x00), 128), LV_PART_MAIN);
            }
            else
            {
                lv_label_set_text(status_label, "ON");
                lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x80, 0x00), 0);
                lv_obj_set_style_arc_color(arc_, lv_color_mix(dark_arc_bg, LV_COLOR_MAKE(0x00, 0x80, 0x00), 128), LV_PART_MAIN);
            }
        }

        if (current_position == 0)
        {
            lv_arc_set_value(arc_, abs(adjusted_sub_position) * 100);
        }
        else
        {
            lv_arc_set_value(arc_, 100 - abs(adjusted_sub_position) * 100);
        }
    }
}

void SwitchApp::updateStateFromSystem(AppState state) {}