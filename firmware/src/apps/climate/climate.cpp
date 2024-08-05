#include "climate.h"

ClimateApp::ClimateApp(SemaphoreHandle_t mutex, char *app_id_, char *friendly_name_, char *entity_id_) : App(mutex)
{
    sprintf(app_id, "%s", app_id_);
    sprintf(friendly_name, "%s", friendly_name_);
    sprintf(entity_id, "%s", entity_id_);

    // TODO update this via some API
    current_temperature = 20;
    target_temperature = 25;
    uint8_t position_nonce = target_temperature;

    // TODO, sync motor config with wanted temp on retrival
    motor_config = PB_SmartKnobConfig{
        target_temperature,
        0,
        position_nonce,
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
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    LV_IMG_DECLARE(x80_thermostat);
    LV_IMG_DECLARE(x40_thermostat);

    big_icon = x80_thermostat;
    small_icon = x40_thermostat;

    initScreen();
    updateTemperatureArc();
}

void ClimateApp::initScreen()
{
    {
        SemaphoreGuard lock(mutex_);

        target_temp_label = lv_label_create(screen);
        lv_obj_set_style_text_font(target_temp_label, &EIGHTTWOXC_48px, 0);
        lv_label_set_text_fmt(target_temp_label, "%dC", target_temperature);
        lv_obj_center(target_temp_label);

        state_label = lv_label_create(screen);
        lv_label_set_text(state_label, "Climate");
        lv_obj_align_to(state_label, target_temp_label, LV_ALIGN_OUT_TOP_MID, 0, -2);

        current_temp_label = lv_label_create(screen);
        lv_label_set_text_fmt(current_temp_label, "%dC", current_temperature);
        lv_obj_align_to(current_temp_label, target_temp_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -4);

        LV_IMG_DECLARE(x20_mode_auto);
        LV_IMG_DECLARE(x20_mode_cool);
        LV_IMG_DECLARE(x20_mode_heat);
        LV_IMG_DECLARE(x20_mode_air);

        mode_auto_icon = lv_img_create(screen);
        lv_img_set_src(mode_auto_icon, &x20_mode_auto);
        lv_obj_add_style(mode_auto_icon, (lv_style_t *)&SK_X20_ICON_STYLE, LV_PART_MAIN);
        lv_obj_align(mode_auto_icon, LV_ALIGN_BOTTOM_MID, -30, -10);
        lv_obj_set_style_img_recolor(mode_auto_icon, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_MAIN);

        mode_cool_icon = lv_img_create(screen);
        lv_img_set_src(mode_cool_icon, &x20_mode_cool);
        lv_obj_add_style(mode_cool_icon, (lv_style_t *)&SK_X20_ICON_STYLE, LV_PART_MAIN);
        lv_obj_align_to(mode_cool_icon, mode_auto_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        mode_heat_icon = lv_img_create(screen);
        lv_img_set_src(mode_heat_icon, &x20_mode_heat);
        lv_obj_add_style(mode_heat_icon, (lv_style_t *)&SK_X20_ICON_STYLE, LV_PART_MAIN);
        lv_obj_align_to(mode_heat_icon, mode_cool_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        mode_air_icon = lv_img_create(screen);
        lv_img_set_src(mode_air_icon, &x20_mode_air);
        lv_obj_add_style(mode_air_icon, (lv_style_t *)&SK_X20_ICON_STYLE, LV_PART_MAIN);
        lv_obj_align_to(mode_air_icon, mode_heat_icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    }
    initTemperatureArc();
}

void ClimateApp::initTemperatureArc()
{
    {
        SemaphoreGuard lock(mutex_);

        temperature_arc = lv_arc_create(screen);

        uint16_t width = 220;
        uint8_t arc_width = ARC_WIDTH;

        lv_obj_remove_style(temperature_arc, NULL, LV_PART_KNOB); // REMOVE KNOB
        lv_obj_set_size(temperature_arc, width, width);
        lv_obj_center(temperature_arc);
        lv_obj_set_style_arc_width(temperature_arc, arc_width, LV_PART_MAIN);
        lv_obj_set_style_arc_width(temperature_arc, arc_width, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(temperature_arc, air_active_color, LV_PART_INDICATOR);
        lv_arc_set_rotation(temperature_arc, ROTATION_ANGLE);
        lv_arc_set_bg_angles(temperature_arc, 0, MAX_ANGLE);

        // Rotation starts at 3o clock to align with LVGL
        uint8_t dot_amount = (CLIMATE_APP_MAX_TEMP - CLIMATE_APP_MIN_TEMP) + 1;
        uint8_t diameter = 6;
        uint8_t rotation = ROTATION_ANGLE;
        uint16_t start_angle = MIN_ANGLE;
        uint16_t end_angle = MAX_ANGLE;

        temperature_dots = (lv_obj_t **)malloc(dot_amount * sizeof(lv_obj_t *));
        assert(temperature_dots != NULL);

        start_angle += rotation;
        end_angle += rotation;

        float angle_step = (float)(end_angle - start_angle) / (dot_amount - 1);

        lv_coord_t screen_width = lv_obj_get_width(screen);
        lv_coord_t screen_height = lv_obj_get_height(screen);
        lv_coord_t center_x = screen_width / 2;
        lv_coord_t center_y = screen_height / 2;

        float radius = (width - arc_width) / 2.0; // Remove arcs width to center dots in the arc

        for (int i = 0; i < dot_amount; i++)
        {

            float angle = (start_angle + i * angle_step) * M_PI / 180.0;

            int x = center_x + radius * cos(angle);
            int y = center_y + radius * sin(angle);

            lv_obj_t *circle = lvDrawCircle(diameter, screen);
            lv_obj_set_pos(circle, x - diameter / 2, y - diameter / 2); // Adjust position to account for the circle's diameter

            uint8_t temp = i + CLIMATE_APP_MIN_TEMP;
            if (temp == current_temperature)
            {
                lv_obj_set_style_bg_color(circle, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_MAIN);
            }
            else if (temp < current_temperature)
            {
                if (target_temperature <= temp && temp < current_temperature)
                    lv_obj_set_style_bg_color(circle, dark_cool_active_color, LV_PART_MAIN);
                else
                    lv_obj_set_style_bg_color(circle, cool_active_color, LV_PART_MAIN);
            }
            else
            {
                if (current_temperature < temp && temp < target_temperature)
                    lv_obj_set_style_bg_color(circle, dark_heat_active_color, LV_PART_MAIN);
                else
                    lv_obj_set_style_bg_color(circle, heat_active_color, LV_PART_MAIN);
            }

            temperature_dots[i] = circle;

            if (i == 0)
            {
                // Get x & y one step before the first dot
                int x_ = center_x + radius * cos(angle - ONE_STEP_ANGLE * DEG_TO_RAD);
                int y_ = center_y + radius * sin(angle - ONE_STEP_ANGLE * DEG_TO_RAD);

                lv_obj_t *min_temp_label = lv_label_create(screen);
                lv_label_set_text_fmt(min_temp_label, "%d", CLIMATE_APP_MIN_TEMP);
                lv_obj_set_style_text_color(min_temp_label, cool_active_color, LV_PART_MAIN);
                lv_obj_update_layout(min_temp_label);
                lv_obj_set_pos(min_temp_label, x_ - lv_obj_get_width(min_temp_label) / 2, y_ - lv_obj_get_height(min_temp_label) / 2);
            }
            else if (i == dot_amount - 1)
            {
                // Get x & y one step after the last dot
                int x_ = center_x + radius * cos(angle + ONE_STEP_ANGLE * DEG_TO_RAD);
                int y_ = center_y + radius * sin(angle + ONE_STEP_ANGLE * DEG_TO_RAD);

                lv_obj_t *max_temp_label = lv_label_create(screen);
                lv_label_set_text_fmt(max_temp_label, "%d", CLIMATE_APP_MAX_TEMP);
                lv_obj_set_style_text_color(max_temp_label, heat_active_color, LV_PART_MAIN);
                lv_obj_update_layout(max_temp_label);
                lv_obj_set_pos(max_temp_label, x_ - lv_obj_get_width(max_temp_label) / 2, y_ - lv_obj_get_height(max_temp_label) / 2);
            }
        }
    }
}

void ClimateApp::updateTemperatureArc()
{
    {
        SemaphoreGuard lock(mutex_);

        // Update temperature labels
        lv_label_set_text_fmt(target_temp_label, "%dC", target_temperature);
        lv_label_set_text_fmt(current_temp_label, "%dC", current_temperature);

        // Update ARC
        uint16_t one_step_angle = ONE_STEP_ANGLE;

        uint16_t angle_current_temp = lerp(current_temperature, CLIMATE_APP_MIN_TEMP, CLIMATE_APP_MAX_TEMP, MIN_ANGLE, MAX_ANGLE);
        uint16_t angle_target_temp = lerp(target_temperature, CLIMATE_APP_MIN_TEMP, CLIMATE_APP_MAX_TEMP, MIN_ANGLE, MAX_ANGLE);

        if (angle_target_temp == angle_current_temp)
        {
            lv_obj_set_style_arc_color(temperature_arc, LV_COLOR_MAKE(0x00, 0xCC, 0x00), LV_PART_INDICATOR);
            lv_arc_set_angles(temperature_arc, angle_current_temp, angle_target_temp + 1);
        }
        else if (angle_target_temp > angle_current_temp)
        {
            lv_obj_set_style_arc_color(temperature_arc, heat_active_color, LV_PART_INDICATOR);
            lv_arc_set_angles(temperature_arc, angle_current_temp, angle_target_temp + 1);
        }
        else
        {
            lv_obj_set_style_arc_color(temperature_arc, cool_active_color, LV_PART_INDICATOR);
            lv_arc_set_angles(temperature_arc, angle_target_temp, angle_current_temp + 1);
        }

        // Update DOTS
        for (int i = 0; i < (CLIMATE_APP_MAX_TEMP - CLIMATE_APP_MIN_TEMP + 1); i++)
        {
            uint8_t temp = i + CLIMATE_APP_MIN_TEMP;
            if (temp == current_temperature)
            {
                lv_obj_set_style_bg_color(temperature_dots[i], LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_MAIN);
            }
            else if (temp < current_temperature)
            {
                if (target_temperature <= temp)
                    lv_obj_set_style_bg_color(temperature_dots[i], dark_cool_active_color, LV_PART_MAIN);
                else
                    lv_obj_set_style_bg_color(temperature_dots[i], cool_active_color, LV_PART_MAIN);
            }
            else
            {
                if (temp <= target_temperature)
                    lv_obj_set_style_bg_color(temperature_dots[i], dark_heat_active_color, LV_PART_MAIN);
                else
                    lv_obj_set_style_bg_color(temperature_dots[i], heat_active_color, LV_PART_MAIN);
            }
        }
    }
}

void ClimateApp::updateModeIcon()
{
    {
        SemaphoreGuard lock(mutex_);

        switch (mode)
        {
        case ClimateAppMode::CLIMATE_AUTO:
            lv_obj_set_style_img_recolor(mode_cool_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_heat_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_air_icon, inactive_color, LV_PART_MAIN);

            if (current_temperature < target_temperature)
            {
                lv_obj_set_style_img_recolor(mode_heat_icon, heat_active_color, LV_PART_MAIN);
                lv_label_set_text(state_label, "Heating");
            }
            else if (current_temperature > target_temperature)
            {
                lv_obj_set_style_img_recolor(mode_cool_icon, cool_active_color, LV_PART_MAIN);
                lv_label_set_text(state_label, "Cooling");
            }
            else if (current_temperature == target_temperature)
            {
                lv_obj_set_style_img_recolor(mode_air_icon, air_active_color, LV_PART_MAIN);
                lv_label_set_text(state_label, "idle");
            }

            lv_obj_set_style_img_recolor(mode_auto_icon, auto_active_color, LV_PART_MAIN);
            break;
        case ClimateAppMode::CLIMATE_COOL:
            lv_obj_set_style_img_recolor(mode_heat_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_air_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_auto_icon, inactive_color, LV_PART_MAIN);

            lv_obj_set_style_img_recolor(mode_cool_icon, cool_active_color, LV_PART_MAIN);
            lv_label_set_text(state_label, "Cooling");
            break;
        case ClimateAppMode::CLIMATE_HEAT:
            lv_obj_set_style_img_recolor(mode_air_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_auto_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_cool_icon, inactive_color, LV_PART_MAIN);

            lv_obj_set_style_img_recolor(mode_heat_icon, heat_active_color, LV_PART_MAIN);
            lv_label_set_text(state_label, "Heating");
            break;
        case ClimateAppMode::CLIMATE_FAN_ONLY:
            lv_obj_set_style_img_recolor(mode_auto_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_cool_icon, inactive_color, LV_PART_MAIN);
            lv_obj_set_style_img_recolor(mode_heat_icon, inactive_color, LV_PART_MAIN);

            lv_obj_set_style_img_recolor(mode_air_icon, air_active_color, LV_PART_MAIN);
            lv_label_set_text(state_label, "idle");
            break;
        }

        lv_obj_align_to(state_label, target_temp_label, LV_ALIGN_OUT_TOP_MID, 0, -2);
        lv_obj_align_to(current_temp_label, target_temp_label, LV_ALIGN_OUT_BOTTOM_MID, 0, -4);
    }
}

EntityStateUpdate ClimateApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;
    if (state_sent_from_hass)
    {
        state_sent_from_hass = false;
        return new_state;
    }
    target_temperature = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = target_temperature;
    motor_config.position = target_temperature;

    adjusted_sub_position = state.sub_position_unit * state.config.position_width_radians;

    if (state.current_position == motor_config.min_position && state.sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && state.sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    if (last_target_temperature != state.current_position || last_mode != mode)
    {
        updateTemperatureArc();

        if (mode == ClimateAppMode::CLIMATE_AUTO)
        {
            updateModeIcon();
        }

        sprintf(new_state.app_id, "%s", app_id);
        // sprintf(new_state.entity_id, "%s", entity_id);
        cJSON *json = cJSON_CreateObject();
        cJSON_AddNumberToObject(json, "mode", mode);
        cJSON_AddNumberToObject(json, "target_temp", target_temperature);
        cJSON_AddNumberToObject(json, "current_temp", current_temperature);

        char *json_string = cJSON_PrintUnformatted(json);
        sprintf(new_state.state, "%s", json_string);

        cJSON_free(json_string);
        cJSON_Delete(json);

        last_mode = mode;
        last_target_temperature = target_temperature;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_CLIMATE);
    }

    //! TEMP FIX VALUE, REMOVE WHEN FIRST STATE VALUE THAT IS SENT ISNT THAT OF THE CURRENT POS FROM MENU WHERE USER INTERACTED TO GET TO THIS APP, create new issue?
    first_run = true;

    return new_state;
}

void ClimateApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
    cJSON *new_state = cJSON_Parse(mqtt_state_update.state);
    cJSON *mode = cJSON_GetObjectItem(new_state, "mode");
    cJSON *target_temp = cJSON_GetObjectItem(new_state, "target_temp");
    cJSON *current_temp = cJSON_GetObjectItem(new_state, "current_temp");

    if (mode != NULL)
    {
        if (mode->valueint >= 0 && mode->valueint < ClimateAppMode::CLIMATE_MODE_COUNT)
        {
            this->mode = static_cast<ClimateAppMode>(mode->valueint);
        }
        else
        {
            LOGE("Invalid mode value: %d", mode->valueint);
            return;
        }
    }

    if (target_temp != NULL)
    {
        target_temperature = target_temp->valueint;
        motor_config.position = target_temperature;
        motor_config.position_nonce = target_temperature;
    }

    if (current_temp != NULL)
    {
        this->current_temperature = current_temp->valueint;
    }

    if (mode != NULL || target_temp != NULL || current_temp != NULL)
    {
        state_sent_from_hass = true;
    }

    cJSON_Delete(new_state);
}

int8_t ClimateApp::navigationNext()
{
    last_mode = mode;
    uint8_t position_nonce = target_temperature;
    switch (mode)
    {
    case ClimateAppMode::CLIMATE_AUTO:
        mode = ClimateAppMode::CLIMATE_COOL;
        motor_config = PB_SmartKnobConfig{
            target_temperature,
            0,
            position_nonce,
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
    case ClimateAppMode::CLIMATE_COOL:
        // todo, check that current temp is more than wanted
        mode = ClimateAppMode::CLIMATE_HEAT;
        motor_config = PB_SmartKnobConfig{
            target_temperature,
            0,
            position_nonce,
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
    case ClimateAppMode::CLIMATE_HEAT:
        // todo, check that current temp is less than wanted
        mode = ClimateAppMode::CLIMATE_FAN_ONLY;
        motor_config = PB_SmartKnobConfig{
            target_temperature,
            0,
            position_nonce,
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
    case ClimateAppMode::CLIMATE_FAN_ONLY:
        mode = ClimateAppMode::CLIMATE_AUTO;
        motor_config = PB_SmartKnobConfig{
            target_temperature,
            0,
            position_nonce,
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
    default:
        break;
    }

    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
    updateModeIcon();

    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}