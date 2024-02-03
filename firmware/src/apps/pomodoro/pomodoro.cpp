#include "pomodoro.h"

// Duration of a Pomodoro in minutes
const long POMODORO_DURATION = 25;

// Duration of a break in minutes
const int BREAK_DURATION = 5;

PomodoroApp::PomodoroApp(TFT_eSprite *spr_) : App(spr_), state(PomodoroState::IDLE), start_time(millis())
{
    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        0,
        0.01,
        0.6,
        1.1,
        "",
        0,
        {},
        0,
        45,
    };

    big_icon = stopwatch_80;
    small_icon = stopwatch_40;
    friendly_name = "Pomodoro";
}

int8_t PomodoroApp::navigationNext()
{
    ESP_LOGD("PomodoroApp", "navigationNext, state: %d", state);
    if (state == PomodoroState::IDLE)
    {
        state = PomodoroState::WORK;
        start_time = millis();
    }
    else if (state == PomodoroState::WORK)
    {
        state = PomodoroState::IDLE;
    }
    else
    {
        state = PomodoroState::IDLE;
    }

    return DONT_NAVIGATE;
}

EntityStateUpdate PomodoroApp::update(AppState state_)
{
    return EntityStateUpdate();
}

TFT_eSprite *PomodoroApp::render()
{
    long now = millis();
    long elapsed = now - start_time;
    long duration = state == PomodoroState::WORK ? POMODORO_DURATION : BREAK_DURATION;
    long total_duration_ms = duration * 60 * 1000;
    long remaining_ms = total_duration_ms - elapsed;
    long total_seconds_remaining = remaining_ms / 1000;
    long minutes_remaining = total_seconds_remaining / 60;
    long seconds_remaining = total_seconds_remaining % 60;

    uint16_t center_h = TFT_HEIGHT / 2;
    uint16_t center_w = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    spr_->setFreeFont(&NDS1210pt7b);

    spr_->fillSprite(TFT_BLACK);

    if (state != PomodoroState::IDLE)
    {

        spr_->fillSprite(state == PomodoroState::WORK ? TFT_RED : TFT_GREENYELLOW);
        spr_->fillSmoothCircle(center_w, center_h, screen_radius - 8, TFT_BLACK, state == PomodoroState::WORK ? TFT_RED : TFT_GREENYELLOW);

        float start_angle = PI / 2;
        float stop_angle = 2.5 * PI - (2.5 * PI - PI / 2) * (1 - ((float)remaining_ms / total_duration_ms));

        for (float r = start_angle; r <= stop_angle; r += PI / 360)
        {
            float ax = screen_radius + (screen_radius - 9) * cosf(r);
            float ay = screen_radius - (screen_radius - 9) * sinf(r);

            float bx = screen_radius + (screen_radius)*cosf(r);
            float by = screen_radius - (screen_radius)*sinf(r);

            spr_->drawWideLine(ax, ay, bx, by, 2, TFT_BLACK, TFT_BLACK);
        }
    }

    if (state == PomodoroState::WORK && remaining_ms <= 0)
    {
        state = PomodoroState::BREAK;
        start_time = now;
    }
    else if (state == PomodoroState::BREAK && remaining_ms <= 0)
    {
        state = PomodoroState::WORK;
        start_time = now;
    }

    if (state == PomodoroState::IDLE)
    {
        spr_->setTextColor(TFT_WHITE);
        spr_->drawString("Press to start", TFT_WIDTH / 2, TFT_HEIGHT / 2);
    }
    else if (state == PomodoroState::WORK)
    {
        spr_->setTextColor(TFT_GREEN);
        spr_->drawString("Work!", screen_radius, screen_radius);

        spr_->drawString("Time Left: ", screen_radius, screen_radius + 20);
        std::string time_str = std::to_string(minutes_remaining) + ":" + std::to_string(seconds_remaining);
        spr_->drawString(time_str.c_str(), screen_radius, screen_radius + 40);

        // SEND STATE PLAY HAPTIC !!!!
    }
    else
    {
        spr_->setTextColor(TFT_RED);

        spr_->drawString("Break!", screen_radius, screen_radius);
        std::string time_str = std::to_string(minutes_remaining) + ":" + std::to_string(seconds_remaining);
        spr_->drawString(time_str.c_str(), screen_radius, screen_radius + 40);
    }

    return spr_;
}