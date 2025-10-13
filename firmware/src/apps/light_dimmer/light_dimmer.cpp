#include "light_dimmer.h"
#include "cJSON.h"
#include <cstring>

LightDimmerApp::LightDimmerApp(SemaphoreHandle_t mutex, AppData app_data) : App(mutex)
{
    strcpy(app_id, app_data.app_id);
    strcpy(friendly_name, app_data.friendly_name);
    strcpy(entity_id, app_data.entity_id);

    strncpy(dimmer_config.id, app_id, sizeof(motor_config.id) - 1);
    strncpy(page_selector_config.id, app_id, sizeof(motor_config.id) - 1);
    strncpy(hue_config.id, app_id, sizeof(motor_config.id) - 1);
    strncpy(temp_config.id, app_id, sizeof(motor_config.id) - 1);

    motor_config = dimmer_config;
    motor_config.position = current_position;

    LV_IMG_DECLARE(x80_light_outline);
    LV_IMG_DECLARE(x40_light_outline);

    big_icon = x80_light_outline;
    small_icon = x40_light_outline;

    page_mgr_ = new LightDimmerPageManager(screen, mutex, app_data);
}

void LightDimmerApp::handleNavigation(NavigationEvent event)
{
    uint8_t prev_nonce = motor_config.position_nonce;
    LightDimmerPages page_num = page_mgr_->getCurrentPageNum();
    switch (event)
    {
    case SHORT:
        switch (page_mgr_->getCurrentPageNum())
        {
        case LIGHT_DIMMER_PAGE:
            dimmer_config = motor_config;
            motor_config = page_selector_config;
            motor_config.position = 0;
            page_mgr_->show(PAGE_SELECTOR);
            break;
        case PAGE_SELECTOR:
            switch (2 + current_position)
            {
            case HUE_PAGE:
                motor_config = hue_config;
                page_mgr_->show(HUE_PAGE);
                break;
            case TEMP_PAGE:
                motor_config = temp_config;
                page_mgr_->show(TEMP_PAGE);
                break;
            // case PRESETS_PAGE:
            //     page_mgr_->show(PRESETS_PAGE);
            //     break;
            default:
                LOGW("No valid page found, page number: %d", 2 + current_position);
                break;
            }
            break;
        case HUE_PAGE:
            motor_config = temp_config;
            page_mgr_->show(TEMP_PAGE);
            break;
        case TEMP_PAGE:
            motor_config = dimmer_config; // TODO: Change to preset config etc
            page_mgr_->show(LIGHT_DIMMER_PAGE);
            break;
        default:
            break;
        }
        break;
    case LONG:
        if (page_num == LIGHT_DIMMER_PAGE)
        {
            return;
        }

        switch (page_num)
        {
        case HUE_PAGE:
            hue_config = motor_config;
            break;
        case TEMP_PAGE:
            temp_config = motor_config;
            break;
            // case PRESETS_PAGE:
            //     page_mgr_->show(PRESETS_PAGE);
            //     break;
        default:
            break;
        }
        motor_config = dimmer_config;
        page_mgr_->show(LIGHT_DIMMER_PAGE);
        break;
    default:
        break;
    }
    motor_config.position_nonce = prev_nonce + 1;
}

int8_t LightDimmerApp::navigationNext()
{
    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}

int8_t LightDimmerApp::navigationBack()
{
    if (page_mgr_->getCurrentPageNum() == LIGHT_DIMMER_PAGE)
    {
        return App::navigationBack();
    }
    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
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

    sub_position_unit = state.sub_position_unit;
    // //! needed to next reload of App
    // motor_config.position_nonce = current_position;
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

    if (last_position != current_position && !first_run)
    {
        page_mgr_->getCurrentPage()->update(mutex_, motor_config.position);

        snprintf(new_state.app_id, sizeof(new_state.app_id) - 1, "%s", app_id);
        snprintf(new_state.entity_id, sizeof(new_state.entity_id) - 1, "%s", entity_id);

        cJSON *json = cJSON_CreateObject();
        cJSON *rgb = cJSON_CreateArray();

        LightDimmerPages page_num = page_mgr_->getCurrentPageNum();

        switch (page_num)
        {
        case LIGHT_DIMMER_PAGE:
            brightness_pos = current_position;
            break;
        case HUE_PAGE:
            hue_pos = current_position;
            break;
        case TEMP_PAGE:
            temp_pos = current_position;
            break;
        default:
            break;
        }

        cJSON_AddBoolToObject(json, "on", brightness_pos > 0);
        cJSON_AddNumberToObject(json, "brightness", round(brightness_pos * 2.55));

        if (page_num == HUE_PAGE)
        {
            int16_t app_hue_deg = hue_pos * skip_degrees_selectable;
            if (app_hue_deg < 0)
                hsv_.h = 360 + app_hue_deg % 360;
            else
                hsv_.h = app_hue_deg % 360;
            lv_color_t rgb_color = lv_color_hsv_to_rgb(hsv_.h, hsv_.s, hsv_.v);
            cJSON_AddItemToArray(rgb, cJSON_CreateNumber(rgb_color.ch.red));
            cJSON_AddItemToArray(rgb, cJSON_CreateNumber(rgb_color.ch.green));
            cJSON_AddItemToArray(rgb, cJSON_CreateNumber(rgb_color.ch.blue));
            cJSON_AddItemToObject(json, "rgb_color", rgb);

            static_cast<DimmerPage *>(page_mgr_->getPage(LIGHT_DIMMER_PAGE))->updateArcColor(rgb_color);
        }
        else if (page_num == TEMP_PAGE)
        {
            int16_t app_temp_deg = (temp_pos * skip_degrees_selectable) % 360;
            uint16_t kelvin;
            uint16_t abs_temp_deg = abs(app_temp_deg);
            float normalized_angle = fmodf(abs(app_temp_deg), 360.0f) / 360.0f; // Normalize to [0, 1]
            if (normalized_angle <= 0.5f)
                kelvin = temp_max + normalized_angle * 2 * (temp_min - temp_max);
            else
                kelvin = temp_min - (normalized_angle - 0.5f) * 2 * (temp_min - temp_max);
            cJSON_AddNumberToObject(json, "color_temp", round(1000000 / kelvin)); // TODO convert kelvin to mired, make sure no values provided by hass are in kelvin.... or make sure all values provided by hass is kelvin wich would be better.

            lv_color_t kelvin_color = kelvinToLvColor(kelvin);
            static_cast<DimmerPage *>(page_mgr_->getPage(LIGHT_DIMMER_PAGE))->updateArcColor(kelvin_color);
        }

        char *json_str = cJSON_PrintUnformatted(json);
        snprintf(new_state.state, sizeof(new_state.state) - 1, "%s", json_str);

        cJSON_free(json_str);
        cJSON_Delete(rgb);
        cJSON_Delete(json);

        snprintf(new_state.app_slug, sizeof(new_state.app_slug) - 1, "%s", APP_SLUG_LIGHT_DIMMER);

        new_state.changed = true;
    }

    // //! TEMP FIX VALUE, REMOVE WHEN FIRST STATE VALUE THAT IS SENT ISNT THAT OF THE CURRENT POS FROM MENU WHERE USER INTERACTED TO GET TO THIS APP, create new issue?
    last_position = current_position;
    first_run = false;
    return new_state;
}

void LightDimmerApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
    cJSON *new_state = cJSON_Parse(mqtt_state_update.state);
    cJSON *on = cJSON_GetObjectItem(new_state, "on");
    cJSON *brightness = cJSON_GetObjectItem(new_state, "brightness");
    cJSON *color_temp = cJSON_GetObjectItem(new_state, "color_temp");
    cJSON *rgb_color = cJSON_GetObjectItem(new_state, "rgb_color");

    LightDimmerPages page_num = page_mgr_->getCurrentPageNum();

    lv_color_hsv_t hsv = {.h = 0, .s = 100, .v = 100};

    if (on != NULL)
    {
        brightness_pos = 3; // 3 = 1%
        if (brightness == NULL && on->valueint == 1)
        {
            current_position = brightness_pos;

            motor_config.position_nonce = current_position;
            motor_config.position = current_position;
        }
        else
        {
            dimmer_config.position = brightness_pos;
            dimmer_config.position_nonce = brightness_pos;
        }
    }

    if (brightness != NULL)
    {
        brightness_pos = round(brightness->valueint / 2.55);
        if (page_num == LIGHT_DIMMER_PAGE && motor_config.position != brightness_pos)
        {
            current_position = brightness_pos;
            last_position = current_position;

            motor_config.position_nonce = current_position;
            motor_config.position = current_position;
        }
        else
        {
            dimmer_config.position = brightness_pos;
            dimmer_config.position_nonce = brightness_pos;
        }
    }

    if (rgb_color != NULL && cJSON_IsNull(rgb_color) == false)
    {
        r = cJSON_GetArrayItem(rgb_color, 0)->valueint;
        g = cJSON_GetArrayItem(rgb_color, 1)->valueint;
        b = cJSON_GetArrayItem(rgb_color, 2)->valueint;

        hsv = lv_color_rgb_to_hsv(r, g, b);

        hue_pos = hsv.h / skip_degrees_selectable;

        if (page_num == HUE_PAGE && motor_config.position != hue_pos)
        {
            current_position = hue_pos;
            last_position = current_position;

            motor_config.position_nonce = hue_pos;
            motor_config.position = hue_pos;
        }
        else
        {
            hue_config.position = hue_pos;
            hue_config.position_nonce = hue_pos;
        }

        lv_color_t rgb_color = lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v);
        static_cast<DimmerPage *>(page_mgr_->getPage(LIGHT_DIMMER_PAGE))->updateArcColor(LV_COLOR_MAKE(r, g, b));
    }

    if (color_temp != NULL && !cJSON_IsNull(color_temp))
    {
        uint16_t mired_value = color_temp->valueint; // TODO MAKE sure all values provided from hass are either mired or kelvin!!!!
        float kelvin = 1000000.0f / mired_value;
        float normalized_kelvin = (kelvin - temp_min) / (temp_max - temp_min);

        temp_pos = -static_cast<int16_t>((1.0f - normalized_kelvin) * 45.0f);

        if (page_num == TEMP_PAGE && motor_config.position != temp_pos)
        {
            current_position = temp_pos;
            last_position = current_position;

            motor_config.position_nonce = temp_pos;
            motor_config.position = temp_pos;
        }
        else
        {
            temp_config.position = temp_pos;
            temp_config.position_nonce = temp_pos;
        }

        lv_color_t kelvin_color = kelvinToLvColor(kelvin);
        static_cast<DimmerPage *>(page_mgr_->getPage(LIGHT_DIMMER_PAGE))->updateArcColor(kelvin_color);
    }

    if (on != NULL || brightness != NULL || color_temp != NULL || rgb_color != NULL)
    {
        state_sent_from_hass = true;
    }

    cJSON_Delete(new_state);

    last_position = current_position;

    page_mgr_->getPage(LIGHT_DIMMER_PAGE)->update(mutex_, brightness_pos);
    page_mgr_->getPage(HUE_PAGE)->update(mutex_, hue_pos);
    page_mgr_->getPage(TEMP_PAGE)->update(mutex_, temp_pos);
}