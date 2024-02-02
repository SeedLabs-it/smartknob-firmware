#include "climate.h"

ClimateApp::ClimateApp(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
{
    // TODO update this via some API
    current_temperature = 22;
    // default wanted temp
    wanted_temperature = 25;

    this->entity_name = entity_name;

    // TODO, sync motor config with wanted temp on retrival
    motor_config = PB_SmartKnobConfig{
        wanted_temperature,
        0,
        wanted_temperature,
        CLIMATE_APP_MIN_TEMP,
        CLIMATE_APP_MAX_TEMP,
        8.225806452 * PI / 120,
        2,
        1,
        1.1,
        "SKDEMO_HVAC",
        0,
        {},
        0,
        27,
    };

    num_positions = CLIMATE_APP_MAX_TEMP - CLIMATE_APP_MIN_TEMP;

    big_icon = hvac_80;
    small_icon = hvac_40;
    friendly_name = "Climate";
}

EntityStateUpdate ClimateApp::updateStateFromKnob(PB_SmartKnobState state)
{
    wanted_temperature = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = wanted_temperature;
    motor_config.position = wanted_temperature;

    adjusted_sub_position = state.sub_position_unit * state.config.position_width_radians;

    if (state.current_position == motor_config.min_position && state.sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && state.sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    EntityStateUpdate new_state;

    // new_state.entity_name = entity_name;
    // new_state.new_value = wanted_temperature * 1.0;

    if (last_wanted_temperature != wanted_temperature)
    {
        last_wanted_temperature = wanted_temperature;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_CLIMATE);
    }

    return new_state;
}

void ClimateApp::updateStateFromSystem(AppState state) {}

int8_t ClimateApp::navigationNext()
{

    mode++;

    if (mode > CLIMATE_APP_MODE_VENTILATION)
    {
        mode = CLIMATE_APP_MODE_AUTO;
    }

    switch (mode)
    {
    case CLIMATE_APP_MODE_AUTO:
        motor_config = PB_SmartKnobConfig{
            wanted_temperature,
            0,
            wanted_temperature,
            CLIMATE_APP_MIN_TEMP,
            CLIMATE_APP_MAX_TEMP,
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
        break;
    case CLIMATE_APP_MODE_COOLING:
        // todo, check that current temp is more than wanted
        motor_config = PB_SmartKnobConfig{
            wanted_temperature,
            0,
            wanted_temperature,
            CLIMATE_APP_MIN_TEMP,
            current_temperature,
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
        break;
    case CLIMATE_APP_MODE_HEATING:
        // todo, check that current temp is less than wanted

        motor_config = PB_SmartKnobConfig{
            wanted_temperature,
            0,
            wanted_temperature,
            current_temperature,
            CLIMATE_APP_MAX_TEMP,
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
        break;
    case CLIMATE_APP_MODE_VENTILATION:
        motor_config = PB_SmartKnobConfig{
            wanted_temperature,
            0,
            wanted_temperature,
            current_temperature,
            current_temperature,
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
        break;
    default:
        break;
    }

    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}

void ClimateApp::drawDots()
{
    // colors
    uint16_t inactive_color = spr_->color565(71, 71, 71);
    uint32_t cooling_color = spr_->color565(80, 100, 200);
    uint32_t cooling_color_dark = spr_->color565(62, 78, 156);
    uint32_t heating_color = spr_->color565(255, 128, 0);
    uint32_t heating_color_dark = spr_->color565(199, 109, 18);

    // screen center
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    // end stops and radians per tick
    float range_radians = (num_positions + 1) * motor_config.position_width_radians;
    float left_bound = PI / 2 + range_radians / 2;
    float right_bound = PI / 2 - range_radians / 2;

    uint32_t dot_color = inactive_color;
    uint8_t dot_radius = 2;

    // draw left tick
    if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_COOLING)
    {
        dot_color = cooling_color_dark;
    }

    float dot_position = left_bound;
    spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(dot_position), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(dot_position), dot_radius, dot_color);

    if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_HEATING)
    {
        dot_color = cooling_color_dark;
    }

    dot_position = right_bound;
    spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(dot_position), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(dot_position), dot_radius, dot_color);

    for (int i = 1; i < num_positions + 1; i++)
    {
        if (min_temp + i < current_temperature)
        {
            if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_COOLING)
            {
                dot_color = cooling_color_dark;
            }
            else
            {
                dot_color = inactive_color;
            }
        }
        else if (min_temp + i == current_temperature)
        {
            dot_color = TFT_WHITE;
        }
        else
        {
            if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_HEATING)
            {
                dot_color = heating_color_dark;
            }
            else
            {
                dot_color = inactive_color;
            }
        }

        float dot_position = left_bound - (range_radians / (num_positions)) * i;
        spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(dot_position), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(dot_position), dot_radius, dot_color);
    }
}

// TODO: make this real temp, when sensor is connected
TFT_eSprite *ClimateApp::render()
{
    uint16_t inactive_color = spr_->color565(71, 71, 71);
    uint32_t cooling_color = spr_->color565(80, 100, 200);
    uint32_t cooling_color_dark = spr_->color565(62, 78, 156);
    uint32_t heating_color = spr_->color565(255, 128, 0);

    // draw division lines
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    float range_radians = (num_positions + 1) * motor_config.position_width_radians;

    float left_bound = PI / 2 + range_radians / 2;
    float right_bound = PI / 2 - range_radians / 2;

    char buf_[16];
    uint32_t text_color;

    // Draw min/max numbers
    float min_number_position = left_bound + (range_radians / num_positions) * 1.5;
    sprintf(buf_, "%d", min_temp);
    if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_COOLING)
    {
        text_color = cooling_color;
    }
    else
    {
        text_color = inactive_color;
    }
    spr_->setTextColor(text_color);
    spr_->setFreeFont(&NDS125_small);
    spr_->drawString(buf_, TFT_WIDTH / 2 + (screen_radius - 10) * cosf(min_number_position), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(min_number_position), 1);

    float max_number_position = right_bound - (range_radians / num_positions) * 1.5;
    sprintf(buf_, "%d", max_temp);
    if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_HEATING)
    {
        text_color = heating_color;
    }
    else
    {
        text_color = inactive_color;
    }
    spr_->setTextColor(text_color);
    spr_->setFreeFont(&NDS125_small);
    spr_->drawString(buf_, TFT_WIDTH / 2 + (screen_radius - 10) * cosf(max_number_position), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(max_number_position), 1);

    uint32_t auto_color = inactive_color;
    uint32_t snowflake_color = inactive_color;
    uint32_t arc_color = inactive_color;
    uint32_t fire_color = inactive_color;
    uint32_t wind_color = inactive_color;

    // set arc
    if (wanted_temperature < current_temperature)
    {
        arc_color = cooling_color;
    }
    else if (wanted_temperature > current_temperature)
    {
        arc_color = heating_color;
    }

    // TODO check for positions

    switch (mode)
    {
    case CLIMATE_APP_MODE_AUTO:
        auto_color = TFT_WHITE;
        break;
    case CLIMATE_APP_MODE_COOLING:
        snowflake_color = TFT_WHITE;
        break;

    case CLIMATE_APP_MODE_HEATING:
        fire_color = TFT_WHITE;
        break;

    case CLIMATE_APP_MODE_VENTILATION:
        wind_color = TFT_WHITE;
        break;

    default:
        break;
    }

    // draw current mode with text and color
    std::string status = "";

    if (wanted_temperature > current_temperature)
    {
        spr_->setTextColor(heating_color);
        if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_HEATING)
        {
            fire_color = heating_color;
        }

        status = "Heating";

        // draw arc of action
        float start_angle = left_bound - (range_radians / num_positions) * (current_temperature - min_temp);
        float wanted_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - min_temp) - adjusted_sub_position;

        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, arc_color);
        }
    }
    else if (wanted_temperature == current_temperature)
    {
        spr_->setTextColor(TFT_WHITE);
        status = "idle";
        if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_VENTILATION)
        {
            wind_color = TFT_GREENYELLOW;
            arc_color = TFT_GREENYELLOW;
        }

        // draw arc of action

        float start_angle = left_bound - (range_radians / num_positions) * (current_temperature - min_temp);
        float wanted_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - min_temp) - adjusted_sub_position;

        if (adjusted_sub_position < 0)
        {
            start_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - min_temp) - adjusted_sub_position;
            wanted_angle = left_bound - (range_radians / num_positions) * (current_temperature - min_temp);
        }

        for (float r = start_angle; r >= wanted_angle; r -= 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, arc_color);
        }
    }
    else
    {
        spr_->setTextColor(cooling_color);
        if (mode == CLIMATE_APP_MODE_AUTO || mode == CLIMATE_APP_MODE_COOLING)
        {
            snowflake_color = cooling_color;
        }
        status = "Cooling";

        // draw arc of action
        // draw arc of action
        float start_angle = left_bound - (range_radians / num_positions) * (current_temperature - min_temp);
        float wanted_angle = left_bound - (range_radians / num_positions) * (wanted_temperature - min_temp) - adjusted_sub_position;

        for (float r = start_angle; r <= wanted_angle; r += 2 * PI / 180)
        {
            spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 10, arc_color);
        }
    }

    spr_->setFreeFont(&NDS1210pt7b);
    spr_->drawString(status.c_str(), TFT_WIDTH / 2, TFT_HEIGHT / 2 - 45, 1);

    // draw wanted temperature
    spr_->setFreeFont(&Pixel62mr11pt7b);
    sprintf(buf_, "%d°C", wanted_temperature);
    spr_->drawString(buf_, TFT_WIDTH / 2, TFT_HEIGHT / 2 - 15, 1);

    // draw current temperature
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS1210pt7b);
    sprintf(buf_, "%d°C", current_temperature);
    spr_->drawString(buf_, TFT_WIDTH / 2, TFT_HEIGHT / 2 + 30, 1);

    uint16_t center = TFT_WIDTH / 2;

    // draw bottom icons

    drawDots();

    uint16_t icon_size = 20;
    uint16_t icon_margin = 3;

    spr_->drawBitmap(center - icon_size * 2 - icon_margin * 3, TFT_HEIGHT - 30, letter_A, icon_size, icon_size, auto_color, TFT_BLACK);
    spr_->drawBitmap(center - icon_size - icon_margin, TFT_HEIGHT - 30, snowflake, icon_size, icon_size, snowflake_color, TFT_BLACK);
    spr_->drawBitmap(center + icon_margin, TFT_HEIGHT - 30, fire, icon_size, icon_size, fire_color, TFT_BLACK);
    spr_->drawBitmap(center + icon_size + icon_margin * 3, TFT_HEIGHT - 30, wind, icon_size, icon_size, wind_color, TFT_BLACK);

    return this->spr_;
};