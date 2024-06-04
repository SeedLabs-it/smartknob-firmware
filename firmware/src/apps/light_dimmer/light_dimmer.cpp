#include "light_dimmer.h"
#include "cJSON.h"
#include <cstring>

#define TFT_HOR_RES 240
#define TFT_VER_RES 240

#define CANVAS_BUF_SIZE ((TFT_HOR_RES * TFT_VER_RES) * 4)
uint32_t *canvas_buf = NULL;

LightDimmerApp::LightDimmerApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id) : App(mutex)
{
    canvas_buf = (uint32_t *)heap_caps_aligned_alloc(16, CANVAS_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(canvas_buf != NULL);

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
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    num_positions = motor_config.max_position - motor_config.min_position;

    LV_IMG_DECLARE(x80_light_outline);
    LV_IMG_DECLARE(x40_light_outline);

    big_icon = x80_light_outline;
    small_icon = x40_light_outline;

    json = cJSON_CreateObject();

    initScreen();
}

void LightDimmerApp::initDimmerScreen()
{
    SemaphoreGuard lock(mutex_);

    dimmer_screen = lv_obj_create(screen);
    lv_obj_remove_style_all(dimmer_screen);
    lv_obj_set_size(dimmer_screen, LV_HOR_RES, LV_VER_RES);
    // lv_obj_add_flag(dimmer_screen, LV_OBJ_FLAG_HIDDEN);

    arc_ = lv_arc_create(dimmer_screen);
    lv_obj_set_size(arc_, 236, 236);
    lv_arc_set_rotation(arc_, 150);
    lv_arc_set_bg_angles(arc_, 0, 240);
    // lv_arc_set_knob_offset(arc_, 0);
    lv_arc_set_value(arc_, 0);
    lv_obj_center(arc_);

    lv_obj_set_style_bg_opa(arc_, LV_OPA_0, LV_PART_KNOB);

    lv_obj_set_style_arc_color(arc_, LV_COLOR_MAKE(0xF5, 0xA4, 0x42), LV_PART_INDICATOR);

    lv_obj_set_style_arc_width(arc_, 18, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_, 18, LV_PART_INDICATOR);

    percentage_label_ = lv_label_create(dimmer_screen);
    char buf_[16];
    sprintf(buf_, "%d%%", current_brightness);
    lv_label_set_text(percentage_label_, buf_);
    lv_obj_set_style_text_font(percentage_label_, &EIGHTTWOXC_48px, 0);
    lv_obj_align(percentage_label_, LV_ALIGN_CENTER, 0, -12);

    lv_obj_t *friendly_name_label = lv_label_create(dimmer_screen);
    lv_label_set_text(friendly_name_label, friendly_name);
    lv_obj_align_to(friendly_name_label, percentage_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
}

#define skip_degrees 5
#define lines_count (360 / skip_degrees)
#define radius 120
#define start_radius (radius - 24)
#define line_length 16

#define deg_1_rad (M_PI / 180.0)

static lv_style_t styles[lines_count];
static lv_obj_t *lines[lines_count];
static lv_point_t points[lines_count][2];

void LightDimmerApp::initHueScreen()
{
    SemaphoreGuard lock(mutex_);

    // create lines for a hue wheel going from 0 to 360
    hue_screen = lv_obj_create(screen);
    lv_obj_remove_style_all(hue_screen);
    lv_obj_set_size(hue_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(hue_screen);
    lv_obj_add_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);

    // static lv_style_t style_line;
    // lv_style_init(&style_line);
    // lv_style_set_line_width(&style_line, 2);                               // Set the line width
    // lv_style_set_line_color(&style_line, LV_COLOR_MAKE(0xFF, 0x00, 0x00)); // Set the line color
    // lv_style_set_line_rounded(&style_line, false);                         // Set the line rounding

    // // Define the points of the line
    // static lv_point_t points[] = {{120, 6}, {120, 20}}; // {{120, 120}}

    // // Create a line and apply the style
    // lv_obj_t *line1 = lv_line_create(hue_screen);
    // lv_line_set_points(line1, points, 2); // Set the points
    // lv_obj_add_style(line1, &style_line, 0);

    // // Align the line
    // lv_obj_align(line1, LV_ALIGN_TOP_LEFT, 0, 0);

    // int line_index = 0;
    for (int i = 0; i < lines_count; i++)
    {
        int angle = (i * skip_degrees + current_position) % 360;
        float x = angle * deg_1_rad;

        lv_coord_t x_start = 120 + start_radius * cos(x);
        lv_coord_t y_start = 120 + start_radius * sin(x);

        lv_coord_t x_end = x_start + line_length * cos(x);
        lv_coord_t y_end = y_start + line_length * sin(x);

        points[i][0] = {x_start, y_start};
        points[i][1] = {x_end, y_end};

        lv_style_init(&styles[i]);
        lv_style_set_line_width(&styles[i], 2);
        lv_style_set_line_color(&styles[i], lv_color_hsv_to_rgb(angle, 100, 100));

        lines[i] = lv_line_create(hue_screen);
        lv_line_set_points(lines[i], points[i], 2);
        lv_obj_add_style(lines[i], &styles[i], 0);

        lv_obj_align(lines[i], LV_ALIGN_TOP_LEFT, 0, 0);
    }

    // updateHueWheel();
}

// void LightDimmerApp::updateHueWheel()
// {
//     for (int i = 0; i < lines_count; i++)
//     {
//         int angle = (i * skip_degrees + current_position) % 360;
//         float x = angle * deg_1_rad;

//         lv_coord_t x_start = 120 + start_radius * cos(x);
//         lv_coord_t y_start = 120 + start_radius * sin(x);

//         lv_coord_t x_end = 120 + (start_radius + line_length) * cos(x);
//         lv_coord_t y_end = 120 + (start_radius + line_length) * sin(x);

//         points[i][0] = {x_start, y_start};
//         points[i][1] = {x_end, y_end};

//         {
//             SemaphoreGuard lock(mutex_);
//             lv_line_set_points(lines[i], points[i], 2);
//         }
//         delay(1); // DONT UN COMMENT ME
//     }
// }

void LightDimmerApp::updateHueWheel()
{
    for (int i = 0; i < lines_count; i++)
    {
        int angle = (i * skip_degrees + (360 - current_position)) % 360; // Reverse the calculation of the angle
        float x = angle * deg_1_rad;

        lv_color_t color = lv_color_hsv_to_rgb(angle, 100, 100);
        lv_style_set_line_color(&styles[i], color);

        {
            SemaphoreGuard lock(mutex_);
            lv_obj_refresh_style(lines[i], LV_PART_MAIN, LV_STYLE_PROP_INV);
        }
        // delay(1); // DONT UN COMMENT ME
    }
}

int8_t LightDimmerApp::navigationNext()
{
    if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
    {
        app_state_mode = LIGHT_DIMMER_APP_MODE_HUE;
        SemaphoreGuard lock(mutex_);
        lv_obj_add_flag(dimmer_screen, LV_OBJ_FLAG_HIDDEN);
        // lv_obj_remove_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);
    }
    else if (app_state_mode == LIGHT_DIMMER_APP_MODE_HUE)
    {
        app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;
        SemaphoreGuard lock(mutex_);
        // lv_obj_remove_flag(dimmer_screen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(dimmer_screen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);
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
        strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
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
            0.5,
            "",
            0,
            {},
            0,
            27,
        };
        strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
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

            // if (app_hue_position % 2 == 0)
            // {
            // SemaphoreGuard lock(mutex_);
            updateHueWheel();
            // }
        }
        else if (app_state_mode == LIGHT_DIMMER_APP_MODE_DIMMER)
        {
            current_brightness = current_position;
            SemaphoreGuard lock(mutex_);
            lv_arc_set_value(arc_, current_brightness);
            char buf_[16];
            sprintf(buf_, "%d%%", current_brightness);
            lv_label_set_text(percentage_label_, buf_);
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

            RGBColor_Custom rgb = uint32ToRGB(ToRGBA(app_hue_position));

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
        return (360 * 100 + position * 2) % 360;
    }
    else
    {
        return (position * 2) % 360;
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

        HSVColor hsv = ToHSV(RGBColor_Custom{r, g, b});

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