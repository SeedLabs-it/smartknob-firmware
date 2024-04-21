#include "light_dimmer.h"
#include "cJSON.h"
#include <cstring>

// utils for screen

#include "picker_mask.h"

LightDimmerApp::LightDimmerApp(TFT_eSprite *spr_, TFT_eSPI *tft_, char *app_id, char *friendly_name, char *entity_id) : App(spr_, tft_)
{
    sprintf(this->app_id, "%s", app_id);
    sprintf(this->friendly_name, "%s", friendly_name);
    sprintf(this->entity_id, "%s", entity_id);

    motor_config_dimmer = PB_SmartKnobConfig{
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

    motor_config_settings = PB_SmartKnobConfig{
        current_settings_position,
        0,
        current_settings_position,
        0,
        3,
        60 * PI / 180,
        1,
        1,
        0.7, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
        "",  // Change the type of app_id from char to char*
        0,
        {},
        0,
        27,
    };

    motor_config_hue = PB_SmartKnobConfig{
        app_hue_position / 4,
        0,
        uint8_t(app_hue_position / 4),
        0,
        -1,
        PI * 2 / 90,
        1,
        1,
        1.1,
        "",
        0,
        {},
        0,
        27,
    };

    // TODO make real kelvins wheel
    motor_config_kelvin = PB_SmartKnobConfig{
        app_hue_position / 2,
        0,
        uint8_t(app_hue_position / 2),
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

    motor_config = motor_config_dimmer;

    num_positions = motor_config.max_position - motor_config.min_position;

    big_icon = light_top_80;
    small_icon = light_top_40;

    json = cJSON_CreateObject();
    loadDimmerMaskSprite();
}

void LightDimmerApp::loadDimmerMaskSprite()
{
    // TFT_eSprite spr = TFT_eSprite(spr_);
    // light_dimmer_mask_spr_ = &spr;
    // light_dimmer_mask_spr_->createSprite(240, 240);
    // light_dimmer_mask_spr_->fillCircle(120, 120, 30, TFT_BLACK);
    // light_dimmer_mask_spr_->createSprite(240, 240);
    // light_dimmer_mask_spr_->fillSprite(TFT_TRANSPARENT);
    // light_dimmer_mask_spr_->fillTriangle(35, 0, 0, 59, 69, 59, TFT_BLACK);
    // light_dimmer_mask_spr_->pushSprite(0, 0, TFT_TRANSPARENT);
}

int8_t LightDimmerApp::navigationNext()
{

    switch (app_state_mode)
    {
    case LIGHT_DIMMER_APP_MODE_DIMMER:
        app_state_mode = LIGHT_DIMMER_APP_MODE_SETTINGS;
        motor_config = motor_config_settings;
        break;
    case LIGHT_DIMMER_APP_MODE_SETTINGS:
        // check what is selected
        app_state_mode = LIGHT_DIMMER_APP_MODE_HUE;
        motor_config = motor_config_hue;
        break;
    case LIGHT_DIMMER_APP_MODE_KELVIN:
        app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;
        motor_config = motor_config_dimmer;
    case LIGHT_DIMMER_APP_MODE_HUE:
        app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;
        motor_config = motor_config_dimmer;
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

    if (app_state_mode == LIGHT_DIMMER_APP_MODE_SETTINGS)
    {
        current_settings_position = state.current_position;
        current_settings_subposition = state.sub_position_unit;
        // TODO rework apps structure
        return new_state;
    }

    current_position = state.current_position;

    sub_position_unit = state.sub_position_unit;
    //! needed to next reload of App
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

        if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
        {
            app_hue_position = calculateAppHuePosition(current_position);

            if (!color_set && calculateAppHuePosition(last_position) != app_hue_position)
            {
                color_set = true;
            }
        }
        else if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
        {
            current_brightness = current_position;
        }

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

        if (color_set)
        {

            RGBColor rgb = uint32ToRGB(ToRGBA(app_hue_position));

            cJSON *rgb_array = cJSON_CreateArray();
            cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb.r));
            cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb.g));
            cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb.b));
            cJSON_AddItemToObject(json, "rgb_color", rgb_array);
        }
        else
        {
            cJSON_AddNullToObject(json, "rgb_color");
        }

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

uint16_t LightDimmerApp::calculateAppHuePosition(uint16_t position)
{
    if (position < 0)
    {
        return (360 * 100 + position * 4) % 360;
    }
    else
    {
        return (position * 4) % 360;
    }
}

void LightDimmerApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
    cJSON *new_state = cJSON_Parse(mqtt_state_update.state);
    cJSON *on = cJSON_GetObjectItem(new_state, "on");
    cJSON *brightness = cJSON_GetObjectItem(new_state, "brightness");
    cJSON *color_temp = cJSON_GetObjectItem(new_state, "color_temp");
    cJSON *rgb_color = cJSON_GetObjectItem(new_state, "rgb_color");

    if (on != NULL)
    {
        is_on = on->valueint;
        if (brightness == NULL && is_on == 1)
        {
            current_brightness = 3; // 3 = 1%

            motor_config.position_nonce = current_position;
            motor_config.position = current_position;
        }
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
        color_set = true;

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

    if (cJSON_IsNull(rgb_color))
    {
        color_set = false;
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

    // cJSON_free(new_state);
    cJSON_Delete(new_state);
}

void LightDimmerApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *LightDimmerApp::renderLightSettings()
{
    uint16_t screen_radius = TFT_WIDTH / 2;
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    int16_t actual_settings_position = current_settings_position % 4;

    int16_t circle_size_r = 34;
    int16_t circle_margin = 20;

    int offset = current_settings_position * (circle_size_r * 2 + circle_margin) + current_settings_subposition * (circle_size_r * 2 + circle_margin) * 0.5;

    int16_t opt_1_x = center_h - offset;
    int16_t opt_1_y = center_v;

    int16_t opt_2_x = center_h + circle_size_r * 2 + circle_margin - offset;
    int16_t opt_2_y = center_v;

    int16_t opt_3_x = center_h + circle_size_r * 2 * 2 + circle_margin * 2 - offset;
    int16_t opt_3_y = center_v;

    int16_t opt_4_x = center_h + circle_size_r * 2 * 3 + circle_margin * 3 - offset;
    int16_t opt_4_y = center_v;

    spr_->fillCircle(opt_1_x, opt_1_y, circle_size_r, TFT_DARKGREY);
    spr_->fillCircle(opt_2_x, opt_2_y, circle_size_r, TFT_DARKGREY);
    spr_->fillCircle(opt_3_x, opt_3_y, circle_size_r, TFT_DARKGREY);
    spr_->fillCircle(opt_4_x, opt_4_y, circle_size_r, TFT_DARKGREY);

    // if (actual_settings_position == 0)
    // {

    //     opt_1_x = center_h - offset;
    //     opt_1_y = center_v;

    //     opt_2_x = center_h;
    //     opt_2_y = center_v - from_center_offset + offset;
    // }
    // else if (actual_settings_position == 1)
    // {
    //     opt_2_x = center_h;
    //     opt_2_y = center_v - offset;

    //     opt_3_x = center_h + from_center_offset - offset;
    //     opt_3_y = center_v;
    // }
    // else if (actual_settings_position == 2)
    // {
    //     opt_3_x = center_h + offset;
    //     opt_3_y = center_v;

    //     opt_4_x = center_h;
    //     opt_4_y = center_v + from_center_offset - offset;
    // }
    // else if (actual_settings_position == 3)
    // {
    //     opt_4_x = center_h;
    //     opt_4_y = center_v + offset;

    //     opt_1_x = center_h - from_center_offset + offset;
    //     opt_1_y = center_v;
    // }

    spr_->setTextDatum(CC_DATUM);

    uint32_t current_color = ToRGBA(0);

    spr_->fillSmoothCircle(center_h, center_v, circle_size_r, TFT_DARKGREY, TFT_BLACK);

    sprintf(buf_, "%s", "color");
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS125_small);
    spr_->fillCircle(opt_1_x, opt_1_y, circle_size_r, TFT_BLACK);
    for (int i = 0; i < circle_size_r - 6; i++)
    {
        current_color = ToRGBA(i * 10);
        spr_->fillCircle(opt_1_x, opt_1_y, circle_size_r - 6 - i, current_color);
    }
    spr_->drawCircle(opt_1_x, opt_1_y, circle_size_r, TFT_WHITE);
    spr_->drawString(buf_, opt_1_x, opt_1_y + circle_size_r + 10, 1);

    sprintf(buf_, "%s", "temp");
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS125_small);
    spr_->fillCircle(opt_2_x, opt_2_y, circle_size_r, TFT_BLACK);
    spr_->fillSmoothCircle(opt_2_x, opt_2_y, circle_size_r - 6, TFT_ORANGE, TFT_BLACK);
    spr_->fillSmoothCircle(opt_2_x, opt_2_y, circle_size_r - 15, TFT_SKYBLUE, TFT_ORANGE);
    spr_->drawCircle(opt_2_x, opt_2_y, circle_size_r, TFT_WHITE);
    spr_->drawString(buf_, opt_2_x, opt_2_y + circle_size_r + 10, 1);

    sprintf(buf_, "%s", "preset");
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS125_small);
    spr_->fillCircle(opt_3_x, opt_3_y, circle_size_r, TFT_BLACK);
    spr_->fillSmoothCircle(opt_3_x, opt_3_y, circle_size_r - 6, TFT_PURPLE, TFT_BLACK);
    spr_->drawCircle(opt_3_x, opt_3_y, circle_size_r, TFT_WHITE);
    spr_->drawString(buf_, opt_3_x, opt_3_y + circle_size_r + 10, 1);

    sprintf(buf_, "%s", "dimmer");
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS125_small);
    spr_->fillCircle(opt_4_x, opt_4_y, circle_size_r, TFT_BLACK);
    spr_->fillSmoothCircle(opt_4_x, opt_4_y, circle_size_r - 6, TFT_LIGHTGREY, TFT_BLACK);
    spr_->drawCircle(opt_4_x, opt_4_y, circle_size_r, TFT_WHITE);
    spr_->drawString(buf_, opt_4_x, opt_4_y + circle_size_r + 10, 1);
    sprintf(buf_, "%d%%", current_brightness);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->drawString(buf_, opt_4_x, opt_4_y, 1);

    return this->spr_;
}

TFT_eSprite *LightDimmerApp::renderKelvinWheel()
{
    return this->spr_;
}

TFT_eSprite *LightDimmerApp::renderHUEWheel()
{
    // TODO: fix jumping circle over 0 crossing
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
    // right_bound = PI / 2 - range_radians - motor_config.position_width_radians;

    uint8_t icon_size = 80;

    uint16_t offset_vertical = 30;

    sprintf(buf_, "%d%%", current_position);

    spr_->fillScreen(DISABLED_COLOR);

    uint16_t segmetns = 360;

    // 1.8 * PI / 180

    float position_in_radians = PI * 2 / (segmetns);

    float segment_position = 0;

    uint32_t segment_color = TFT_WHITE;

    // shift wheel by current position
    // left_bound = left_bound - position_in_radians * app_hue_position;

    for (int i = 0; i < segmetns; i++)
    {
        segment_position = left_bound - position_in_radians * i;
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

    for (int i = 0; i < black_border_size; i++)
    {
        spr_->drawCircle(center_h, center_v, TFT_WIDTH / 2 - i, color_black);
        spr_->drawCircle(center_h + 1, center_v, TFT_WIDTH / 2 - i, color_black);
        spr_->drawCircle(center_h - 1, center_v, TFT_WIDTH / 2 - i, color_black);
        spr_->drawCircle(center_h, center_v + 1, TFT_WIDTH / 2 - i, color_black);
        spr_->drawCircle(center_h, center_v - 1, TFT_WIDTH / 2 - i, color_black);
    }

    spr_->fillSmoothCircle(center_h, center_v, TFT_WIDTH / 2 - black_border_size - color_strip_size, color_black, color_black);

    uint8_t dot_x = TFT_WIDTH / 2 + (screen_radius - black_border_size - color_strip_size / 2) * cosf(left_bound - position_in_radians * app_hue_position);
    uint8_t dot_y = TFT_HEIGHT / 2 - (screen_radius - black_border_size - color_strip_size / 2) * sinf(left_bound - position_in_radians * app_hue_position);

    spr_->fillCircle(dot_x, dot_y, (color_strip_size + 10) / 2, current_color);
    spr_->drawCircle(dot_x, dot_y, (color_strip_size + 10) / 2, TFT_BLACK);
    spr_->drawCircle(dot_x, dot_y, (color_strip_size + 10) / 2 + 1, TFT_BLACK);

    HEXColor current_color_hex = hToHEX(app_hue_position);

    sprintf(buf_, "%d%%", current_brightness);
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS1210pt7b);

    spr_->drawString(friendly_name, center_h, center_v + 20, 1);
    spr_->setFreeFont(&Pixel62mr11pt7b);
    spr_->drawString(buf_, center_h, center_v - 22, 1);

    return this->spr_;
}

TFT_eSprite *LightDimmerApp::render()
{

    switch (app_state_mode)
    {
    case LIGHT_DIMMER_APP_MODE_HUE:
        return renderHUEWheel();
    case LIGHT_DIMMER_APP_MODE_SETTINGS:
        return renderLightSettings();
    default:
        break;
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

    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_BLACK);
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS1210pt7b);

    spr_->drawString(friendly_name, center_h, center_v + 20, 1);
    spr_->setFreeFont(&Pixel62mr11pt7b);
    spr_->drawString(buf_, center_h, center_v - 22, 1);

    uint32_t current_color = ToRGBA(app_hue_position);

    if (current_position > 0)
    {
        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            // draw the arc
            spr_->fillSmoothCircle(
                TFT_WIDTH / 2 + (screen_radius - black_border_size - color_strip_size / 2) * cosf(r),
                TFT_HEIGHT / 2 - (screen_radius - black_border_size - color_strip_size / 2) * sinf(r),
                color_strip_size / 2 - 2, // for smoth line
                current_color);
        }
    }

    return this->spr_;
};