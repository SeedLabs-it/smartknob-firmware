#include "light_dimmer.h"
#include "cJSON.h"
#include <cstring>

#define TFT_HOR_RES 240 // TODO inherit this from config rather than redefine.
#define TFT_VER_RES 240 // TODO inherit this from config rather than redefine.

#define CANVAS_BUF_SIZE ((TFT_HOR_RES * TFT_VER_RES) * 4)
uint32_t *canvas_buf = NULL;

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
    motor_config.position_nonce = current_position;

    // // num_positions = motor_config.max_position - motor_config.min_position;

    LV_IMG_DECLARE(x80_light_outline);
    LV_IMG_DECLARE(x40_light_outline);

    big_icon = x80_light_outline;
    small_icon = x40_light_outline;

    // json = cJSON_CreateObject();

    // initScreen();

    page_mgr_ = new LightDimmerPageManager(screen, mutex, app_data);
}

void LightDimmerApp::handleNavigation(NavigationEvent event)
{
    switch (event)
    {
    case SHORT:
        switch (page_mgr_->getCurrentPageNum())
        {
        case LIGHT_DIMMER_PAGE:
            // page_mgr_->getCurrentPage()->handleNavigation(event);
            motor_config = page_selector_config;
            strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
            page_mgr_->show(PAGE_SELECTOR);
            break;
        case PAGE_SELECTOR:
            // current_position = brightness_pos;
            switch (((LightDimmerPages)(2 + current_position)))
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
            motor_config.position = current_position;
            motor_config.position_nonce = current_position;
            break;
        default:
            break;
        }
        break;
    case LONG:
        switch (page_mgr_->getCurrentPageNum())
        {
        case LIGHT_DIMMER_PAGE:
            LOGE("LightDimmerPage::handleNavigation");
            LOGE("SHOULD RETURN TO MAIN MENU");
            break;
        default:
            motor_config = dimmer_config;
            motor_config.position = current_position;
            motor_config.position_nonce = current_position;
            strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
            page_mgr_->show(LIGHT_DIMMER_PAGE);
            break;
        }
        break;
    default:
        break;
    }
}

// update motor config trought handleNavigation
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
        // SemaphoreGuard lock(mutex_); // TODO move down into page somehow
        page_mgr_->getCurrentPage()->update(mutex_, current_position);

        new_state.changed = true;
    }

    //! TEMP FIX VALUE, REMOVE WHEN FIRST STATE VALUE THAT IS SENT ISNT THAT OF THE CURRENT POS FROM MENU WHERE USER INTERACTED TO GET TO THIS APP, create new issue?
    last_position = current_position;
    first_run = true;
    return new_state;
}

void LightDimmerApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
    // cJSON *new_state = cJSON_Parse(mqtt_state_update.state);
    // cJSON *on = cJSON_GetObjectItem(new_state, "on");
    // cJSON *brightness = cJSON_GetObjectItem(new_state, "brightness");
    // cJSON *color_temp = cJSON_GetObjectItem(new_state, "color_temp");
    // cJSON *rgb_color = cJSON_GetObjectItem(new_state, "rgb_color");

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

    // if (brightness != NULL)
    // {
    //     current_brightness = round(brightness->valueint / 2.55);
    //     if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER && current_brightness != current_position)
    //     {
    //         current_position = current_brightness;
    //         last_position = current_position;

    //         motor_config.position_nonce = current_position;
    //         motor_config.position = current_position;
    //     }
    // }

    // if (rgb_color != NULL && cJSON_IsNull(rgb_color) == 0)
    // {
    //     color_set = true;

    //     r = cJSON_GetArrayItem(rgb_color, 0)->valueint;
    //     g = cJSON_GetArrayItem(rgb_color, 1)->valueint;
    //     b = cJSON_GetArrayItem(rgb_color, 2)->valueint;

    //     hsv = lv_color_rgb_to_hsv(r, g, b);

    //     app_hue_position = hsv.h / skip_degrees;

    //     if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE && app_hue_position != current_position)
    //     {
    //         current_position = app_hue_position;

    //         motor_config.position_nonce = app_hue_position;
    //         motor_config.position = app_hue_position;
    //     }
    // }

    // if (cJSON_IsNull(rgb_color))
    // {
    //     color_set = false;
    // }

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

    // if (on != NULL || brightness != NULL || color_temp != NULL || rgb_color != NULL)
    // {
    //     state_sent_from_hass = true;
    // }

    // cJSON_Delete(new_state);

    // last_position = current_position;

    // {
    //     SemaphoreGuard lock(mutex_);

    //     if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
    //     {

    //         if (current_brightness == 0)
    //         {
    //             lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_PART_MAIN);
    //             lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
    //         }
    //         else
    //         {
    //             lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0x47, 0x27, 0x01), LV_PART_MAIN);
    //             lv_obj_set_style_arc_color(arc_, lv_color_mix(dark_arc_bg, LV_COLOR_MAKE(0x47, 0x27, 0x01), 128), LV_PART_MAIN);
    //         }

    //         if (color_set)
    //         {
    //             lv_obj_set_style_arc_color(arc_, LV_COLOR_MAKE(r, g, b), LV_PART_INDICATOR);
    //         }

    //         lv_arc_set_value(arc_, current_brightness);
    //         char buf_[16];
    //         sprintf(buf_, "%d%%", current_brightness);
    //         lv_label_set_text(percentage_label_, buf_);
    //     }
    //     // else if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
    //     // {
    //     //     LOGE("HUE: %d", hsv.h);
    //     //     lv_obj_set_style_bg_color(selected_hue, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), LV_PART_MAIN);
    //     // }
    // }
}