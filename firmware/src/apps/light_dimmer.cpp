#include "light_dimmer.h"
#include "cJSON.h"
#include <cstring>

LightDimmerApp::LightDimmerApp(TFT_eSprite *spr_, std::string app_id, std::string friendly_name) : App(spr_)
{
    this->app_id = app_id.c_str();
    this->friendly_name_ = friendly_name.c_str();

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        100,
        3.6 / 2 * PI / 180,
        1,
        1,
        1.1,
        "SKDEMO_Light_dimmer",
        0,
        {},
        0,
        27,
    };

    num_positions = motor_config.max_position - motor_config.min_position;

    big_icon = light_top_80;
    small_icon = light_top_40;
}

EntityStateUpdate LightDimmerApp::updateStateFromKnob(PB_SmartKnobState state)
{
    current_position = state.current_position;
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

    EntityStateUpdate new_state;

    if (last_position != current_position)
    {
        new_state.app_id = app_id;
        cJSON *json = cJSON_CreateObject();
        cJSON_AddNumberToObject(json, "brightness", int(current_position * 2.55));
        cJSON_AddNumberToObject(json, "color_temp", 0);
        cJSON *rgb_array = cJSON_CreateArray();
        cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(255));
        cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(255));
        cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(255));
        cJSON_AddItemToObject(json, "rgb_color", rgb_array);

        sprintf(new_state.state, "%s", cJSON_PrintUnformatted(json));

        cJSON_Delete(json);

        last_position = current_position;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_LIGHT_DIMMER);
    }

    return new_state;
}

void LightDimmerApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *LightDimmerApp::render()
{
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

    left_bound = PI / 2;
    right_bound = PI / 2 - range_radians - motor_config.position_width_radians;

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
        background_color = on_background;
        foreground_color = on_lamp_color;
        dot_color = on_background;
    }

    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, background_color);
    spr_->setTextColor(foreground_color);
    spr_->setFreeFont(&NDS1210pt7b);

    spr_->drawString(friendly_name_.c_str(), center_h, center_v + 20, 1);
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
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, dot_color);
    }

    return this->spr_;
};