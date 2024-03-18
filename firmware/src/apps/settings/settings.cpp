#include "settings.h"

#include "../../root_task.h"

SettingsApp::SettingsApp(TFT_eSprite *spr_) : App(spr_)
{
    sprintf(app_id, "%s", "settings");
    sprintf(entity_id, "%s", "settings");
    sprintf(friendly_name, "%s", "Settings");

    sprintf(fw_version, "FW: %s", "v0.1-demo");
    sprintf(room, "%s", "Kitchen");

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        4,
        90 * PI / 180,
        1,
        1,
        0.55,
        "SKDEMO_Settings",
        0,
        {},
        0,
        70,
    };

    num_positions = motor_config.max_position - motor_config.min_position;

    // set icons for menu
    big_icon = settings_80;
    small_icon = settings_40;
    // friendly_name = "Settings";

    startup_ms = millis();
}

EntityStateUpdate SettingsApp::updateStateFromKnob(PB_SmartKnobState state)
{
    if (motor_calibration_started)
    {
        return EntityStateUpdate{};
    }

    current_position = state.current_position;

    // // needed to next reload of App
    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    adjusted_sub_position = state.sub_position_unit * state.config.position_width_radians;

    if (state.current_position == motor_config.min_position && state.sub_position_unit < 0)
    {
        adjusted_sub_position = -logf(1 - state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }
    else if (state.current_position == motor_config.max_position && state.sub_position_unit > 0)
    {
        adjusted_sub_position = logf(1 + state.sub_position_unit * motor_config.position_width_radians / 5 / PI * 180) * 5 * PI / 180;
    }

    return EntityStateUpdate{};
}

void SettingsApp::updateStateFromSystem(AppState state)
{
    sprintf(ip_address, "%s", state.connectivity_state.ip_address);
    sprintf(ssid, "%s", state.connectivity_state.ssid);

    connectivity_state.is_connected = state.connectivity_state.is_connected;
    connectivity_state.signal_strength = state.connectivity_state.signal_strength;
    connectivity_state.signal_strenth_status = state.connectivity_state.signal_strenth_status;

    proximity_state = state.proximiti_state;

    // // needed to next reload of App
    // motor_config.position_nonce = current_volume_position;
    // motor_config.position = current_volume_position;
}

int8_t SettingsApp::navigationNext()
{
    // todo: make it a const
    if (current_position == 4)
    {
        if (!motor_calibration_started)
        {
            motor_calibration_started = true;
            motor_calibration_requested_ms = millis();
            motor_calibration_expected_finish_ms = motor_calibration_requested_ms + (calibration_time_s + calibration_safety_time_s) * 1000;
            motor_calibration_expected_event_send_ms = motor_calibration_requested_ms + calibration_safety_time_s * 1000;
        }
    }
    return DONT_NAVIGATE;
}

// int8_t SettingsApp::navigationBack()
// {
//     // todo: make it a const
//     if (current_position == 4)
//     {
//         if (!motor_calibration_finished)
//         {
//             motor_calibration_started = true;
//             motor_calibration_requested_ms = millis();
//         }
//     }
//     return DONT_NAVIGATE;
// }

void SettingsApp::renderViewMotorCalibration()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_HEIGHT / 2;

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->drawString("Motor calibration", center_h, center_v - 50, 1);

    if (!motor_calibration_started)
    {
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString("Press to start", center_h, center_v + 50, 1);
    }
    else
    {

        if (motor_calibration_event_sent)
        {
            spr_->setTextColor(TFT_ORANGE);
        }

        spr_->setFreeFont(&NDS1210pt7b);
        sprintf(buf_, "%ds", (motor_calibration_expected_finish_ms - millis()) / 1000);
        spr_->drawString(buf_, center_h, center_v, 1);

        spr_->drawString("Don't touch it", center_h, center_v + 50, 1);
    }
}

TFT_eSprite *SettingsApp::render()
{
    // section to update inner state

    if (motor_calibration_started)
    {
        if (!motor_calibration_event_sent && motor_calibration_expected_event_send_ms < millis())
        {
            // TODO: send event
            motor_calibration_event_sent = true;
            delete_me_TriggerMotorCalibration();
        }

        if (motor_calibration_expected_finish_ms < millis())
        {
            motor_calibration_finished = true;
            motor_calibration_started = false;
            motor_calibration_event_sent = false;
        }
    }

    // rendering

    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    float range_radians = (num_positions + 1) * motor_config.position_width_radians;

    float left_bound = PI / 2 + range_radians / 2;
    float right_bound = PI / 2 - range_radians / 2;

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_HEIGHT / 2;

    uint16_t screen_radius = TFT_WIDTH / 2;

    float raw_angle = left_bound - (current_position - motor_config.min_position) * motor_config.position_width_radians;
    float adjusted_angle = raw_angle - adjusted_sub_position;

    char buf_[128];

    if (current_position == 0)
    {
        // spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_PURPLE);

        spr_->setTextColor(TFT_WHITE);
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString("Settings", center_h, 20, 1);

        spr_->setTextColor(TFT_WHITE);
        spr_->setFreeFont(&NDS1210pt7b);

        // draw line to separate title
        spr_->fillRect(0, 45, TFT_WIDTH, 2, DISABLED_COLOR);

        uint8_t wifi_icon_size = 40;
        uint32_t wifi_icon_color = TFT_WHITE;
        if (!connectivity_state.is_connected)
        {
            wifi_icon_color = DISABLED_COLOR;
        }

        spr_->drawBitmap(20, 75, wifi_40, wifi_icon_size, wifi_icon_size, wifi_icon_color, TFT_BLACK);

        // wifi_30

        // log(connectivity_state.ssid.c_str());

        int16_t signal_strength = connectivity_state.signal_strength;
        std::string signal_strength_text = "";

        uint32_t signal_strength_color = TFT_GREEN;

        switch (connectivity_state.signal_strenth_status)
        {
        case 0:
            signal_strength_color = TFT_GREEN;
            signal_strength_text = "Excellent";
            break;
        case 1:
            signal_strength_color = TFT_YELLOW;
            signal_strength_text = "Good";
            break;
        case 2:
            signal_strength_color = TFT_ORANGE;
            signal_strength_text = "Fair";
            break;
        case 3:
            signal_strength_color = TFT_RED;
            signal_strength_text = "Poor";
            break;
        case 4:
            signal_strength_color = TFT_RED;
            signal_strength_text = "No signal";
            break;
        default:
            break;
        }

        if (connectivity_state.is_connected)
        {
            spr_->setTextColor(signal_strength_color);
            spr_->setFreeFont(&NDS1210pt7b);
            spr_->drawString(signal_strength_text.c_str(), center_h, 94, 1);

            spr_->setTextColor(TFT_WHITE);
            spr_->setFreeFont(&NDS1210pt7b);
            sprintf(buf_, "%ddb", signal_strength);
            spr_->drawString(buf_, center_h + 75, 94, 1);

            spr_->setTextColor(TFT_WHITE);
            spr_->setFreeFont(&NDS1210pt7b);
            sprintf(buf_, "SSID: %s", ssid);
            spr_->drawString(buf_, center_h, 130, 1);

            spr_->setTextColor(TFT_WHITE);
            spr_->setFreeFont(&NDS1210pt7b);
            sprintf(buf_, "IP: %s", ip_address);
            spr_->drawString(buf_, center_h, 160, 1);
        }
        else
        {

            signal_strength_color = DISABLED_COLOR;
            signal_strength_text = "Disconnected";

            spr_->setTextColor(signal_strength_color);
            spr_->setFreeFont(&NDS1210pt7b);
            spr_->drawString(signal_strength_text.c_str(), center_h + 30, 94, 1);
        }

        spr_->setTextColor(TFT_WHITE);
        sprintf(buf_, "%dmm | %d", proximity_state.RangeMilliMeter, proximity_state.RangeStatus);
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString(buf_, center_h, 190, 1);
    }
    else if (current_position == 1)
    {
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_BLACK);

        spr_->setTextColor(TFT_WHITE);
        sprintf(buf_, "%s", "Proximity");
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString(buf_, center_h, 20, 1);

        spr_->setTextDatum(CR_DATUM);
        spr_->setTextColor(TFT_WHITE);
        sprintf(buf_, "%d", proximity_state.RangeStatus);
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString(buf_, TFT_WIDTH - 5, center_v, 1);

        uint16_t max_r = 90;
        uint16_t min_r = 5;
        uint32_t circle_color = spr_->color565(255, 0, 0);

        uint16_t max_distance_mm = 500;

        uint16_t r = min_r;

        if (proximity_state.RangeStatus <= 2 && proximity_state.RangeMilliMeter < max_distance_mm)
        {
            // max 1m
            float adjusted_r = max_r - (float)proximity_state.RangeMilliMeter / (float)max_distance_mm * max_r;
            r = adjusted_r;
            if (r < min_r)
            {
                r = min_r;
            }

            spr_->fillCircle(center_h, center_v, r, TFT_RED);
        }
        else
        {
            spr_->drawCircle(center_h, center_v, r, TFT_RED);
        }

        spr_->setTextDatum(CC_DATUM);
        spr_->setTextColor(TFT_WHITE);
        sprintf(buf_, "%dmm", proximity_state.RangeMilliMeter);
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString(buf_, center_h, TFT_HEIGHT - 20, 1);
    }
    else if (current_position == 2)
    {

        // Loading Animation
        uint32_t background_color = TFT_BLACK;
        uint32_t text_color = TFT_WHITE;

        startup_diff_ms = millis() - startup_ms;
        uint32_t current_tick_time = startup_diff_ms / 24 % 120; // 120 ticks of 24ms each

        // redraw canvas
        spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, background_color);
        spr_->setTextDatum(CC_DATUM);
        spr_->setTextColor(text_color);
        sprintf(buf_, "%s", "Loading");
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString(buf_, center_h, center_v, 1); // string, xpos, ypos, font

        // draw animation

        spr_->drawCircle(center_h, center_v, current_tick_time, TFT_GREENYELLOW);

        spr_->drawCircle(center_h, center_v, (current_tick_time + 60) % 120, TFT_GREENYELLOW); // concentric circle
    }
    else if (current_position == 3)
    {

        // flip colors every second
        uint32_t background_color = TFT_BLACK;
        uint32_t text_color = TFT_WHITE;

        startup_diff_ms = millis() - startup_ms;

        switch ((startup_diff_ms / 1000) % 3)
        {
        case 0:
            background_color = TFT_BLACK;
            text_color = TFT_WHITE;
            break;
        case 1:
            background_color = TFT_ORANGE;
            text_color = TFT_WHITE;
            break;
        case 2:
            background_color = TFT_DARKCYAN;
            text_color = TFT_WHITE;
            break;

        default:
            break;
        }

        // screen tearing test
        sprintf(buf_, "%s", "Screen test");
        spr_->fillCircle(TFT_WIDTH / 2, TFT_HEIGHT / 2, TFT_WIDTH / 2, background_color);

        spr_->setTextDatum(CC_DATUM);
        spr_->setTextColor(text_color);
        spr_->setFreeFont(&NDS1210pt7b);
        spr_->drawString(buf_, center_h, 25, 1);
    }
    else if (current_position == 4)
    {
        renderViewMotorCalibration();
    }
    // else if (current_position == 5)
    // {
    //     // ben10 easter egg
    //     uint32_t backroung_green = TFT_GREENYELLOW;
    //     uint32_t center_margin = 20;
    //     spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, backroung_green);
    //     spr_->fillTriangle(0, 0, TFT_WIDTH / 2 - center_margin / 2, center_v, 0, TFT_HEIGHT, TFT_BLACK);
    //     spr_->fillTriangle(TFT_WIDTH, 0, TFT_WIDTH / 2 + center_margin / 2, center_v, TFT_WIDTH, TFT_HEIGHT, TFT_BLACK);

    //     uint8_t black_border = 20;
    //     spr_->fillTriangle(0, 0 + black_border, TFT_WIDTH / 2 - center_margin / 2 - black_border, center_v, 0, TFT_HEIGHT - black_border, DISABLED_COLOR);
    //     spr_->fillTriangle(TFT_WIDTH, 0 + black_border, TFT_WIDTH / 2 + center_margin / 2 + black_border, center_v, TFT_WIDTH, TFT_HEIGHT - black_border, DISABLED_COLOR);

    //     for (float r = 0; r >= -6.3; r -= 2 * PI / 180)
    //     {
    //         spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 11, TFT_BLACK);
    //     }

    //     if (adjusted_angle > left_bound)
    //     {
    //         adjusted_angle = left_bound;
    //     }
    //     else if (adjusted_angle < right_bound)
    //     {
    //         adjusted_angle = right_bound;
    //     }

    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, backroung_green);
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - PI / 2), 5, backroung_green);
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 2 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 2 * PI / 2), 5, backroung_green);
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 3 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 3 * PI / 2), 5, backroung_green);
    // }
    // else if (current_position >= 5)
    // {
    //     // ben10 easter egg
    //     uint32_t ben_10_green = TFT_GREENYELLOW;

    //     uint8_t triangle_offset_h = 20;
    //     uint8_t triangle_offset_v = 40;
    //     uint32_t backroung_grey = spr_->color565(0, 0, 0);
    //     spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, ben_10_green);

    //     uint32_t icon_width = 75;
    //     uint32_t icon_height = 174;

    //     if (current_position == 2)
    //     {
    //         spr_->drawBitmap(center_h - icon_width / 2, center_v - icon_height / 2, omnitrix_char_1, icon_width, icon_height, TFT_BLACK, ben_10_green);
    //     }
    //     else if (current_position == 3)
    //     {
    //         icon_width = 85;
    //         spr_->drawBitmap(center_h - icon_width / 2, center_v - icon_height / 2, omnitrix_char_2, icon_width, icon_height, TFT_BLACK, ben_10_green);
    //     }

    //     spr_->fillTriangle(0, 0, center_h + triangle_offset_h, 0, 0, center_v + triangle_offset_v, TFT_BLACK);
    //     spr_->fillTriangle(TFT_WIDTH, 0, center_h - triangle_offset_h, 0, TFT_WIDTH, center_v + triangle_offset_v, TFT_BLACK);

    //     spr_->fillTriangle(0, TFT_HEIGHT, center_h + triangle_offset_h, TFT_HEIGHT, 0, center_v - triangle_offset_v, TFT_BLACK);
    //     spr_->fillTriangle(TFT_WIDTH, TFT_HEIGHT, center_h - triangle_offset_h, TFT_HEIGHT, TFT_WIDTH, center_v - triangle_offset_v, TFT_BLACK);

    //     // spr_->fillTriangle(center_h, triangle_offset_v, TFT_WIDTH - triangle_offset_h, center_v, triangle_offset_h, center_v, ben_10_green);
    //     // spr_->fillTriangle(center_h, TFT_HEIGHT - triangle_offset_v, TFT_WIDTH - triangle_offset_h, center_v, triangle_offset_h, center_v, ben_10_green);

    //     // draw controls

    //     for (float r = 0; r >= -6.3; r -= 2 * PI / 180)
    //     {
    //         spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(r), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(r), 11, TFT_BLACK);
    //     }

    //     if (adjusted_angle > left_bound)
    //     {
    //         adjusted_angle = left_bound;
    //     }
    //     else if (adjusted_angle < right_bound)
    //     {
    //         adjusted_angle = right_bound;
    //     }

    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle), 5, ben_10_green);
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - PI / 2), 5, ben_10_green);
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 2 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 2 * PI / 2), 5, ben_10_green);
    //     spr_->fillCircle(TFT_WIDTH / 2 + (screen_radius - 10) * cosf(adjusted_angle - 3 * PI / 2), TFT_HEIGHT / 2 - (screen_radius - 10) * sinf(adjusted_angle - 3 * PI / 2), 5, ben_10_green);
    // }

    // Draw navigation menu.
    // num_position represents the total pages
    // current_position is the index of the page, starting from 0.
    uint32_t menu_item_color;
    uint8_t menu_item_diameter = 6;
    uint8_t position_circle_radius = screen_radius - menu_item_diameter; // the radius of the circle where you want the dots to lay.
    float degree_per_item = 8 * PI / 180;                                // the degree (angle) between two points in radian
    float center_point_degree = 270 * PI / 180;                          //
    float menu_starting_angle = center_point_degree - ((num_positions * degree_per_item) / 2);

    for (uint16_t i = 0; i <= num_positions; i++)
    {
        // draw a circle
        if (current_position == i)
        {
            menu_item_color = TFT_GREENYELLOW;
        }
        else
        {
            menu_item_color = TFT_WHITE;
        }
        // polar coordinates
        spr_->fillCircle(screen_radius + (position_circle_radius * cosf(menu_starting_angle + degree_per_item * i)), screen_radius - position_circle_radius * sinf(menu_starting_angle + degree_per_item * i), menu_item_diameter / 2, menu_item_color);
    }

    return this->spr_;
};