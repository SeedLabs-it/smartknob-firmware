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

    motor_config = PB_SmartKnobConfig{
        current_position,
        0,
        (uint8_t)current_position,
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
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    // // num_positions = motor_config.max_position - motor_config.min_position;

    LV_IMG_DECLARE(x80_light_outline);
    LV_IMG_DECLARE(x40_light_outline);

    big_icon = x80_light_outline;
    small_icon = x40_light_outline;

    // json = cJSON_CreateObject();

    // initScreen();

    page_mgr_ = new LightDimmerPageManager(screen, mutex, app_data);
}

// void LightDimmerApp::initDimmerScreen()
// {
//     SemaphoreGuard lock(mutex_);

//     dimmer_screen = lv_obj_create(screen);
//     lv_obj_remove_style_all(dimmer_screen);
//     lv_obj_set_size(dimmer_screen, LV_HOR_RES, LV_VER_RES);

//     arc_ = lv_arc_create(dimmer_screen);
//     lv_obj_set_size(arc_, 220, 220);
//     lv_arc_set_rotation(arc_, 150);
//     lv_arc_set_bg_angles(arc_, 0, 240);
//     lv_arc_set_value(arc_, 0);
//     lv_obj_center(arc_);

//     lv_obj_set_style_bg_color(arc_, dark_arc_bg, LV_PART_KNOB);
//     lv_obj_set_style_arc_color(arc_, dark_arc_bg, LV_PART_MAIN);
//     lv_obj_set_style_arc_color(arc_, LV_COLOR_MAKE(0xF5, 0xA4, 0x42), LV_PART_INDICATOR);

//     lv_obj_set_style_arc_width(arc_, 18, LV_PART_MAIN);
//     lv_obj_set_style_arc_width(arc_, 18, LV_PART_INDICATOR);
//     lv_obj_set_style_pad_all(arc_, -6, LV_PART_KNOB);

//     percentage_label_ = lv_label_create(dimmer_screen);
//     char buf_[16];
//     sprintf(buf_, "%d%%", current_brightness);
//     lv_label_set_text(percentage_label_, buf_);
//     lv_obj_set_style_text_font(percentage_label_, &roboto_light_mono_48pt, 0);
//     lv_obj_align(percentage_label_, LV_ALIGN_CENTER, 0, -12);

//     lv_obj_t *friendly_name_label = lv_label_create(dimmer_screen);
//     lv_label_set_text(friendly_name_label, friendly_name);
//     lv_obj_align_to(friendly_name_label, percentage_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
// }

// #define skip_degrees 1                   // distance between two lines in degrees [TODO] refactor this should be the space between lines, not the distance between the start of a line and the other (To account of >1px line)
#define skip_degrees_selectable 4 // distance between two lines in degrees [TODO] refactor this should be the space between lines, not the distance between the start of a line and the other (To account of >1px line)
// #define lines_count (360 / skip_degrees) // number of lines in a 360 circle. // [TODO] refactor, this should account of line thickness + space_between_lines.
// #define lines_count_selectable (360 / skip_degrees_selectable)
// #define distance_from_center 70 // distance from center (pixel) from where the line starts (and goes outword towards the edge of the display)
// #define line_length 120         // length of the ticker line (in pixel)
// #define deg_1_rad (M_PI / 180.0)

// #define selector_radius 80 // Radius of where to place selector dot.

// static lv_style_t styles[lines_count];

// static lv_style_t styles_indicator[lines_count];

// static lv_obj_t *lines[lines_count];
// static lv_point_t points[lines_count][2];

// static lv_obj_t *lines_indicator[lines_count];
// static lv_point_t points_indicator[lines_count][2];

// static lv_style_t selector_style;
// static lv_point_t selector_line_points[3][2];
// static lv_obj_t *selector_lines[3];

// static lv_obj_t *meter;
// static lv_meter_indicator_t *indic_hue;

// void LightDimmerApp::initHueScreen()
// {

//     hue_screen = lv_obj_create(screen);
//     lv_obj_remove_style_all(hue_screen);
//     lv_obj_set_size(hue_screen, LV_HOR_RES, LV_VER_RES);
//     lv_obj_center(hue_screen);
//     lv_obj_add_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);

//     meter = lv_meter_create(hue_screen);
//     lv_obj_remove_style_all(meter);
//     lv_obj_set_size(meter, 210, 210);
//     lv_obj_center(meter);

//     lv_meter_scale_t *scale_hue = lv_meter_add_scale(meter);
//     lv_meter_set_scale_ticks(meter, scale_hue, lines_count - 1, 3, 12, lv_palette_main(LV_PALETTE_GREY));
//     lv_meter_set_scale_range(meter, scale_hue, 0, lines_count, 360, 270);

//     // indic_hue = lv_meter_add_needle_line(meter, scale_hue, 2, LV_COLOR_MAKE(0x00, 0x00, 0x00), -20);

//     lv_obj_add_event_cb(meter, meter_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, &hsv);

//     selected_hue = lvDrawCircle(9, hue_screen);
//     lv_obj_set_style_bg_color(selected_hue, lv_color_hsv_to_rgb(app_hue_position * skip_degrees_selectable, 100, 100), LV_PART_MAIN);
//     lv_obj_align(selected_hue, LV_ALIGN_CENTER, selector_radius * cos(deg_1_rad * (270 + app_hue_position)), selector_radius * sin(deg_1_rad * (270 + app_hue_position)));
// }

// void LightDimmerApp::updateHueWheel()
// {
// }

void LightDimmerApp::handleNavigation(NavigationEvent event)
{
    switch (event)
    {
    case SHORT:
        switch (page_mgr_->getCurrentPageNum())
        {
        case LIGHT_DIMMER_PAGE:
            // page_mgr_->getCurrentPage()->handleNavigation(event);
            motor_config = PB_SmartKnobConfig{
                0,
                0,
                0,
                0,
                LIGHT_DIMMER_PAGE_COUNT - 2, // -2 dimmer doesnt count as its not selectable (default page)
                35 * PI / 180,
                1,
                1,
                1.1,
                "",
                0,
                {},
                0,
                27,
            };
            strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
            page_mgr_->show(PAGE_SELECTOR);
            break;
        case PAGE_SELECTOR:
            // current_position = brightness_pos;
            page_mgr_->show((LightDimmerPages)(2 + current_position));
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
            motor_config = PB_SmartKnobConfig{
                current_position,
                0,
                (uint8_t)current_position,
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
    return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
}

// int8_t LightDimmerApp::navigationNext()
// {
//     // if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
//     // {
//     //     app_state_mode = LIGHT_DIMMER_APP_MODE_HUE;
//     //     SemaphoreGuard lock(mutex_);
//     //     lv_obj_add_flag(dimmer_screen, LV_OBJ_FLAG_HIDDEN);
//     //     // lv_obj_clear_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);
//     // }
//     // // else if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
//     // // {
//     // //     app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;
//     // //     SemaphoreGuard lock(mutex_);
//     // //     lv_obj_clear_flag(dimmer_screen, LV_OBJ_FLAG_HIDDEN);
//     // //     // lv_obj_add_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);
//     // // }

//     // switch (app_state_mode)
//     // {
//     // case LIGHT_DIMMER_APP_MODE_DIMMER:
//     //     motor_config = PB_SmartKnobConfig{
//     //         current_brightness,
//     //         0,
//     //         current_brightness,
//     //         0,
//     //         100,
//     //         2.4 * PI / 180,
//     //         1,
//     //         1,
//     //         1.1,
//     //         "",
//     //         0,
//     //         {},
//     //         0,
//     //         27,
//     //     };
//     //     strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
//     //     break;
//     // // case LIGHT_DIMMER_APP_MODE_HUE:
//     // //     // todo, check that current temp is more than wanted
//     // //     motor_config = PB_SmartKnobConfig{
//     // //         app_hue_position,
//     // //         0,
//     // //         app_hue_position,
//     // //         0,
//     // //         -1,
//     // //         skip_degrees_selectable * PI / 180,
//     // //         1,
//     // //         1,
//     // //         0.5,
//     // //         "",
//     // //         0,
//     // //         {},
//     // //         0,
//     // //         27,
//     // //     };
//     // //     strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
//     // //     break;
//     // default:
//     //     break;
//     // }
//     return DONT_NAVIGATE_UPDATE_MOTOR_CONFIG;
// }

// int8_t LightDimmerApp::navigationBack()
// {
//     // first_run = false;
//     // if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
//     // {
//     //     navigationNext();
//     // }

//     return App::navigationBack();
// }

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
        SemaphoreGuard lock(mutex_);
        page_mgr_->getCurrentPage()->update(current_position);

        new_state.changed = true;
    }
    //     // if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
    //     // {
    //     //     if (!color_set && last_position != app_hue_position)
    //     //     {
    //     //         color_set = true;
    //     //     }

    //     //     if (current_position >= 0)
    //     //     {
    //     //         app_hue_position = current_position % lines_count_selectable;
    //     //     }
    //     //     else
    //     //     {
    //     //         app_hue_position = lines_count_selectable + (current_position % lines_count_selectable);
    //     //     }
    //     //     // lv_meter_set_indicator_value(meter, indic_hue, app_hue_position);
    //     //     float app_hue_deg = app_hue_position * skip_degrees_selectable;

    //     //     hsv.h = app_hue_deg;
    //     //     hsv.s = 100;
    //     //     hsv.v = 100;

    //     //     float u = deg_1_rad * (270 + app_hue_deg);

    //     //     uint16_t x = selector_radius * cos(u);
    //     //     uint16_t y = selector_radius * sin(u);

    //     //     SemaphoreGuard lock(mutex_);
    //     //     lv_obj_set_style_bg_color(selected_hue, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), LV_PART_MAIN);
    //     //     lv_obj_align(selected_hue, LV_ALIGN_CENTER, x, y);
    //     // }
    //     if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
    //     {
    //         current_brightness = current_position;
    //         SemaphoreGuard lock(mutex_);
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
    //             lv_obj_set_style_arc_color(arc_, lv_color_hsv_to_rgb(app_hue_position * skip_degrees_selectable, 100, 100), LV_PART_INDICATOR);
    //         }

    //         lv_arc_set_value(arc_, current_brightness);
    //         char buf_[16];
    //         sprintf(buf_, "%d%%", current_brightness);
    //         lv_label_set_text(percentage_label_, buf_);
    //     }

    //     if (current_brightness == 0)
    //     {
    //         is_on = false;
    //     }
    //     else
    //     {
    //         is_on = true;
    //     }

    //     sprintf(new_state.app_id, "%s", app_id);
    //     sprintf(new_state.entity_id, "%s", entity_id);

    //     cJSON *json = cJSON_CreateObject();

    //     cJSON_AddBoolToObject(json, "on", is_on);
    //     cJSON_AddNumberToObject(json, "brightness", round(current_brightness * 2.55));
    //     cJSON_AddNumberToObject(json, "color_temp", 0);

    //     if (color_set)
    //     {
    //         lv_color_t rgb_color = lv_color_hsv_to_rgb(app_hue_position * skip_degrees_selectable, 100, 100);

    //         cJSON *rgb_array = cJSON_CreateArray();
    //         cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb_color.ch.red));
    //         cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb_color.ch.green));
    //         cJSON_AddItemToArray(rgb_array, cJSON_CreateNumber(rgb_color.ch.blue));
    //         cJSON_AddItemToObject(json, "rgb_color", rgb_array);
    //     }
    //     else
    //     {
    //         cJSON_AddNullToObject(json, "rgb_color");
    //     }

    //     char *json_string = cJSON_PrintUnformatted(json);
    //     sprintf(new_state.state, "%s", json_string);

    //     cJSON_free(json_string);
    //     cJSON_Delete(json);

    //     last_position = current_position;
    //     new_state.changed = true;
    //     sprintf(new_state.app_slug, "%s", APP_SLUG_LIGHT_DIMMER);
    // }

    //! TEMP FIX VALUE, REMOVE WHEN FIRST STATE VALUE THAT IS SENT ISNT THAT OF THE CURRENT POS FROM MENU WHERE USER INTERACTED TO GET TO THIS APP, create new issue?
    last_position = current_position;
    first_run = true;
    return new_state;
}

// int8_t LightDimmerApp::calculateAppHuePosition(int8_t position)
// {
//     if (position < 0)
//     {
//         return (360 * 100 + position * 2) % 360;
//     }
//     else
//     {
//         return (position * 2) % 360;
//     }
// }

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