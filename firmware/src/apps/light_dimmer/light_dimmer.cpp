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

// update motor config trough handleNavigation
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

        // switch (page_mgr_->getCurrentPageNum())
        // {
        // case LIGHT_DIMMER_PAGE:
        //     new_state.state = cJSON_CreateObject();
        //     cJSON_AddNumberToObject(new_state.state, "brightness", current_position);
        //     break;
        // case HUE_PAGE:
        //     new_state.state = cJSON_CreateObject();
        //     cJSON_AddNumberToObject(new_state.state, "hue", current_position);
        //     break;
        // case TEMP_PAGE:
        //     new_state.state = cJSON_CreateObject();
        //     cJSON_AddNumberToObject(new_state.state, "color_temp", current_position);
        //     break;
        // default:
        //     break;
        // }

        snprintf(new_state.app_id, sizeof(new_state.app_id) - 1, "%s", app_id);
        snprintf(new_state.entity_id, sizeof(new_state.entity_id) - 1, "%s", entity_id);

        cJSON *json = cJSON_CreateObject();
        cJSON *rgb = cJSON_CreateArray();

        switch (page_mgr_->getCurrentPageNum())
        {
        case LIGHT_DIMMER_PAGE:
            cJSON_AddBoolToObject(json, "on", current_position > 0);
            cJSON_AddNumberToObject(json, "brightness", round(current_position * 2.55));
            cJSON_AddNumberToObject(json, "color_temp", 0);
            cJSON_AddItemToObject(json, "rgb_color", cJSON_CreateNull());
            break;
        case HUE_PAGE:
            // cJSON_AddItemToArray(rgb, cJSON_CreateNumber(r));
            // cJSON_AddItemToArray(rgb, cJSON_CreateNumber(g));
            // cJSON_AddItemToArray(rgb, cJSON_CreateNumber(b));
            // cJSON_AddItemToObject(json, "rgb_color", rgb);
            break;
        case TEMP_PAGE:
            // cJSON_AddNumberToObject(json, "color_temp", current_position);
            break;
        default:
            break;
        }

        char *json_str = cJSON_PrintUnformatted(json);
        snprintf(new_state.state, sizeof(new_state.state) - 1, "%s", json_str);

        cJSON_free(json_str);
        cJSON_Delete(rgb);
        cJSON_Delete(json);

        snprintf(new_state.app_slug, sizeof(new_state.app_slug) - 1, "%s", APP_SLUG_LIGHT_SWITCH);

        new_state.changed = true;
    }

    //! TEMP FIX VALUE, REMOVE WHEN FIRST STATE VALUE THAT IS SENT ISNT THAT OF THE CURRENT POS FROM MENU WHERE USER INTERACTED TO GET TO THIS APP, create new issue?
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

    int16_t brightness_pos = 0;
    int16_t hue_pos = 0;
    int16_t temp_pos = 0;

    // if (on != NULL)
    // {
    //     is_on = on->valueint;
    //     if (brightness == NULL && is_on == 1)
    //     {
    //         current_brightness = 3; // 3 = 1%

    //         motor_config.position_nonce = current_position;
    //         motor_config.position = current_position;
    //     }
    // }

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

        // // current_brightness = round(brightness->valueint / 2.55);
        // if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER && current_brightness != current_position)
        // {
        //     current_position = current_brightness;
        //     last_position = current_position;

        //     motor_config.position_nonce = current_position;
        //     motor_config.position = current_position;
        // }
    }

    if (rgb_color != NULL && cJSON_IsNull(rgb_color) == 0)
    {
        // color_set = true;

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

        // if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE && app_hue_position != current_position)
        // {
        //     current_position = app_hue_position;

        //     motor_config.position_nonce = app_hue_position;
        //     motor_config.position = app_hue_position;
        // }
    }

    if (cJSON_IsNull(rgb_color))
    {
        color_set = false;
    }

    // if (brightness != NULL || (color_temp != NULL && cJSON_IsNull(color_temp) == 0) || (rgb_color != NULL && cJSON_IsNull(rgb_color) == 0))
    // {
    //     adjusted_sub_position = sub_position_unit * motor_config.position_width_radians;

    //     if (current_position == motor_config.min_position && sub_position_unit < 0)
    //     {
    //         adjusted_sub_position = -logf(1 - sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    //     }
    //     else if (current_position == motor_config.max_position && sub_position_unit > 0)
    //     {
    //         adjusted_sub_position = logf(1 + sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    //     }
    // }

    if (on != NULL || brightness != NULL || color_temp != NULL || rgb_color != NULL)
    {
        state_sent_from_hass = true;
    }

    cJSON_Delete(new_state);

    last_position = current_position;

    {
        // SemaphoreGuard lock(mutex_);

        // if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
        // {
        page_mgr_->getPage(LIGHT_DIMMER_PAGE)->update(mutex_, brightness_pos);
        page_mgr_->getPage(HUE_PAGE)->update(mutex_, hue_pos);
        page_mgr_->getPage(TEMP_PAGE)->update(mutex_, temp_pos);
        if (page_num == LIGHT_DIMMER_PAGE)
        {
            // page_mgr_->getPage(LIGHT_DIMMER_PAGE)->update(mutex_, brightness_pos);
            // page_mgr_->getPage(HUE_PAGE)->update(mutex_, hue_pos);
            // page_mgr_->getPage(TEMP_PAGE)->update(mutex_, temp_pos);
            // if (brightness_val == 0)
            // {
            //     lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_PART_MAIN);
            //     lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
            // }
            // else
            // {
            //     lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x47, 0x27, 0x01), LV_PART_MAIN);
            //     lv_obj_set_style_arc_color(arc_, lv_color_mix(dark_arc_bg, LV_COLOR_MAKE(0x47, 0x27, 0x01), 128), LV_PART_MAIN);
            // }

            // if (color_set)
            // {
            //     lv_obj_set_style_arc_color(arc_, LV_COLOR_MAKE(r, g, b), LV_PART_INDICATOR);
            // }

            // lv_arc_set_value(arc_, current_brightness);
            // char buf_[16];
            // sprintf(buf_, "%d%%", current_brightness);
            // lv_label_set_text(percentage_label_, buf_);
        }
        // else if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
        // {
        //     LOGE("HUE: %d", hsv.h);
        //     lv_obj_set_style_bg_color(selected_hue, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), LV_PART_MAIN);
        // }
    }
}