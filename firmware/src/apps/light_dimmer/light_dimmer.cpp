#include "light_dimmer.h"
#include "cJSON.h"
#include <cstring>

LightDimmerApp::LightDimmerApp(TFT_eSprite *spr_, char *app_id, char *friendly_name, char *entity_id) : App(spr_)
{
    sprintf(this->app_id, "%s", app_id);
    sprintf(this->friendly_name, "%s", friendly_name);
    sprintf(this->entity_id, "%s", entity_id);

    motor_config = PB_SmartKnobConfig{
        current_brightness,
        0,
        current_brightness,
        0,
        100,
        2.4 * PI / 180,
        1,
        1,
        1.1,
        "",
        0,
        {},
        0,
        27,
    };

    num_positions = motor_config.max_position - motor_config.min_position;

    big_icon = light_top_80;
    small_icon = light_top_40;

    json = cJSON_CreateObject();
}

int8_t LightDimmerApp::navigationNext()
{
    if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
    {
        app_state_mode = LIGHT_DIMMER_APP_MODE_HUE;
    }
    else if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
    {
        app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;
    }

    switch (app_state_mode)
    {
    case LIGHT_DIMMER_APP_MODE_DIMMER:
        motor_config = PB_SmartKnobConfig{
            current_brightness,
            0,
            current_brightness,
            0,
            100,
            2.4 * PI / 180,
            1,
            1,
            1.1,
            "",
            0,
            {},
            0,
            27,
        };
        break;
    case LIGHT_DIMMER_APP_MODE_HUE:
        // todo, check that current temp is more than wanted
        motor_config = PB_SmartKnobConfig{
            app_hue_position / 2,
            0,
            app_hue_position / 2,
            0,
            -1,
            PI * 2 / 180,
            1,
            1,
            1.1,
            "",
            0,
            {},
            0,
            27,
        };
        break;
    default:
        break;
    }
    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}

int8_t LightDimmerApp::navigationBack()
{
    first_run = false;
    if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
    {
        navigationNext();
    }

    return App::navigationBack();
}

EntityStateUpdate LightDimmerApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;

    if (state_sent_from_hass)
    {
        state_sent_from_hass = false;
        return new_state;
    }

    current_position = state.current_position;

    if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
    {
        if (current_position < 0)
        {
            app_hue_position = (360 * 100 + current_position * 2) % 360;
        }
        else
        {
            app_hue_position = (current_position * 2) % 360;
        }
    }
    else if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
    {
        if (current_position > 100)
        {
            current_position = 100;
        }
        if (current_position < 0)
        {
            current_position = 0;
        }
        current_brightness = current_position;
    }

    sub_position_unit = state.sub_position_unit;
    // // needed to next reload of App
    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    adjusted_sub_position = sub_position_unit * motor_config.position_width_radians;

    if (state.current_position == motor_config.min_position && sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    if (last_position != current_position && first_run)
    {
        if (current_brightness == 0)
        {
            is_on = false;
        }
        else
        {
            is_on = true;
        }

        sprintf(new_state.app_id, "%s", app_id);
        sprintf(new_state.entity_id, "%s", entity_id);

        cJSON *json = cJSON_CreateObject();

        cJSON_AddBoolToObject(json, "on", is_on);
        cJSON_AddNumberToObject(json, "brightness", round(current_brightness * 2.55));
        cJSON_AddNumberToObject(json, "color_temp", 0);

        RGBColor rgb = uint32ToRGB(ToRGBA(app_hue_position));

        cJSON *rgb_array = cJSON_CreateArray();
        cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb.r));
        cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb.g));
        cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb.b));
        cJSON_AddItemToObject(json, "rgb_color", rgb_array);

        char *json_string = cJSON_PrintUnformatted(json);
        sprintf(new_state.state, "%s", json_string);

        cJSON_free(json_string);
        cJSON_Delete(json);

        last_position = current_position;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_LIGHT_DIMMER);
    }

    //! TEMP FIX VALUE, REMOVE WHEN FIRST STATE VALUE THAT IS SENT ISNT THAT OF THE CURRENT POS FROM MENU WHERE USER INTERACTED TO GET TO THIS APP, create new issue?
    first_run = true;
    return new_state;
}

void LightDimmerApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{

    cJSON *on = cJSON_GetObjectItem(mqtt_state_update.state, "on");
    cJSON *brightness = cJSON_GetObjectItem(mqtt_state_update.state, "brightness");
    cJSON *color_temp = cJSON_GetObjectItem(mqtt_state_update.state, "color_temp");
    cJSON *rgb_color = cJSON_GetObjectItem(mqtt_state_update.state, "rgb_color");

    if (on != NULL)
    {
        is_on = on->valueint;
    }

    if (brightness != NULL)
    {
        current_brightness = round(brightness->valueint / 2.55);
        if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER && current_brightness != current_position)
        {
            current_position = current_brightness;
            last_position = current_position;

            motor_config.position_nonce = current_position;
            motor_config.position = current_position;
        }
    }

    // if (color_temp != NULL && cJSON_IsNull(color_temp) == 0
    // {
    //     app_hue_position = color_temp->valueint / 2.55;
    // }

    if (rgb_color != NULL && cJSON_IsNull(rgb_color) == 0)
    {
        uint8_t r = cJSON_GetArrayItem(rgb_color, 0)->valueint;
        uint8_t g = cJSON_GetArrayItem(rgb_color, 1)->valueint;
        uint8_t b = cJSON_GetArrayItem(rgb_color, 2)->valueint;

        HSVColor hsv = ToHSV(RGBColor{r, g, b});

        app_hue_position = hsv.h;

        if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE && app_hue_position != current_position)
        {
            app_hue_position = hsv.h / 2; // UGLY FIX?
            current_position = app_hue_position;

            motor_config.position_nonce = app_hue_position;
            motor_config.position = app_hue_position;
        }
    }

    if (brightness != NULL || (color_temp != NULL && cJSON_IsNull(color_temp) == 0) || (rgb_color != NULL && cJSON_IsNull(rgb_color) == 0))
    {
        adjusted_sub_position = sub_position_unit * motor_config.position_width_radians;

        if (current_position == motor_config.min_position && sub_position_unit < 0)
        {
            adjusted_sub_position = -logf(1 - sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
        }
        else if (current_position == motor_config.max_position && sub_position_unit > 0)
        {
            adjusted_sub_position = logf(1 + sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
        }
    }

    if (on != NULL || brightness != NULL || color_temp != NULL || rgb_color != NULL)
    {
        state_sent_from_hass = true;
    }

    // cJSON_Delete(new_state);
}

void LightDimmerApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *LightDimmerApp::renderHUEWheel()
{
    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);
    uint16_t color_black = spr_->color565(0, 0, 0);
    uint16_t color_light_grey = spr_->color565(200, 200, 200);

    uint16_t screen_radius = TFT_WIDTH / 2;
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint32_t off_background = spr_->color565(0, 0, 0);
    uint32_t off_lamp_color = spr_->color565(150, 150, 150);
    uint32_t on_background = spr_->color565(71, 39, 1);
    uint32_t on_lamp_color = spr_->color565(245, 164, 66);
    uint32_t background_color;
    uint32_t foreground_color;
    float left_bound = PI / 2;
    float right_bound = 0;
    float range_radians = (motor_config.max_position - motor_config.min_position) * motor_config.position_width_radians;

    left_bound = PI / 2;
    right_bound = PI / 2 - range_radians - motor_config.position_width_radians;

    uint8_t icon_size = 80;

    uint16_t offset_vertical = 30;

    char buf_[16];
    sprintf(buf_, "%d%%", current_position);

    spr_->fillScreen(DISABLED_COLOR);

    uint16_t segmetns = 360;

    // 1.8 * PI / 180

    float position_in_radians = PI * 2 / (segmetns);

    float segment_position = 0;

    uint32_t segment_color = TFT_WHITE;

    // shift wheel by current position
    left_bound = left_bound - position_in_radians * app_hue_position;

    for (int i = 0; i < segmetns; i++)
    {
        segment_position = left_bound + position_in_radians * i;
        segment_color = ToRGBA(i);
        spr_->fillTriangle(
            TFT_WIDTH / 2 + (screen_radius + 10) * cosf(segment_position - position_in_radians / 2),
            TFT_HEIGHT / 2 - (screen_radius + 10) * sinf(segment_position - position_in_radians / 2),
            TFT_WIDTH / 2 + (screen_radius + 10) * cosf(segment_position + position_in_radians / 2),
            TFT_HEIGHT / 2 - (screen_radius + 10) * sinf(segment_position + position_in_radians / 2),
            center_h,
            center_v,
            segment_color);
    }

    uint32_t current_color = ToRGBA(app_hue_position);
    spr_->fillSmoothCircle(center_h, center_v, 80, color_black, color_black);

    spr_->fillTriangle(center_h, center_v - 70, center_h - 10, center_v - 55, center_h + 10, center_v - 55, current_color);

    HEXColor current_color_hex = hToHEX(app_hue_position);

    sprintf(buf_, "#%02X%02X%02X", current_color_hex.r, current_color_hex.g, current_color_hex.b);
    spr_->setTextColor(current_color);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->drawString(buf_, center_h, center_v, 1);

    sprintf(buf_, "HEX", app_hue_position);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(color_light_grey);
    spr_->drawString(buf_, center_h, center_v + 30, 1);

    return this->spr_;
}

TFT_eSprite *LightDimmerApp::render()
{

    if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
    {
        return renderHUEWheel();
    }

    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);
    uint16_t screen_radius = TFT_WIDTH / 2;
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint32_t off_background = spr_->color565(0, 0, 0);
    uint32_t off_lamp_color = spr_->color565(150, 150, 150);
    uint32_t on_background = spr_->color565(71, 39, 1);
    uint32_t on_lamp_color = spr_->color565(245, 164, 66);
    uint32_t background_color;
    uint32_t foreground_color;
    uint32_t dot_color = TFT_WHITE;
    float left_bound = PI / 2;
    float right_bound = 0;
    float range_radians = (motor_config.max_position - motor_config.min_position) * motor_config.position_width_radians;

    left_bound = PI / 2 + range_radians / 2;
    right_bound = PI / 2 - range_radians / 2 - motor_config.position_width_radians;

    float raw_angle = left_bound - (current_position - motor_config.min_position) * motor_config.position_width_radians;
    float adjusted_angle = raw_angle - adjusted_sub_position;

    uint8_t icon_size = 80;

    uint16_t offset_vertical = 30;

    char buf_[6];
    sprintf(buf_, "%d%%", current_position);

    float start_angle = left_bound;
    float wanted_angle = right_bound;

    wanted_angle = left_bound - (range_radians / num_positions) * (current_position - motor_config.min_position) - adjusted_sub_position - motor_config.position_width_radians;
    if (wanted_angle < right_bound - motor_config.position_width_radians)
    {
        wanted_angle = right_bound - motor_config.position_width_radians;
    }

    if (wanted_angle > left_bound)
    {
        wanted_angle = left_bound;
    }

    if (current_position <= 0)
    {
        background_color = off_background;
        foreground_color = off_lamp_color;
        dot_color = off_background;
        strcpy(buf_, "OFF");
    }
    else
    {
        if (is_on)
        {
            background_color = on_background;
        }
        else
        {
            background_color = off_background;
        }
        foreground_color = on_lamp_color;
        dot_color = on_background;
    }

    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, background_color);
    spr_->setTextColor(foreground_color);
    spr_->setFreeFont(&NDS1210pt7b);

    spr_->drawString(friendly_name, center_h, center_v + 20, 1);
    spr_->setFreeFont(&Pixel62mr11pt7b);
    spr_->drawString(buf_, center_h, center_v - 22, 1);

    if (current_position > 0)
    {
        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            // draw the arc
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, foreground_color);
        }
        // there is some jittering on adjusted_angle that might push the dot outside the arc.
        // need to  bound it on the right side. On the left side it's already turned off by
        // the current_position > 0 condition.

        if (adjusted_angle < right_bound)
        {
            adjusted_angle = right_bound;
        }
        spr_->fillSmoothCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, dot_color, foreground_color);
    }

    return this->spr_;
};