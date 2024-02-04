#include "pomodoro.h"

// Duration of a Pomodoro in minutes
const long POMODORO_DURATION = 25;

// Duration of a break in minutes
const int BREAK_DURATION = 5;

PomodoroApp::PomodoroApp(TFT_eSprite *spr_) : App(spr_), state(PomodoroState::IDLE), start_time(millis()), state_changed(false)
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

EntityStateUpdate PomodoroApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;

    if (state_changed)
    {
        new_state.play_haptic = true;
        state_changed = false;
    }
    return new_state;
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

    spr_->setTextDatum(CC_DATUM);
    spr_->setFreeFont(&NDS1210pt7b);

    uint16_t font_height = spr_->fontHeight(1);

    uint16_t text_begin = screen_radius - font_height * 2;

    spr_->fillSprite(TFT_BLACK);

    uint32_t work_color = TFT_RED;
    uint32_t break_color = TFT_BLUE;

    if (state != PomodoroState::IDLE)
    {
        float start_angle = PI / 2;
        float stop_angle = 2.5 * PI - (2.5 * PI - PI / 2) * (1 - ((float)remaining_ms / total_duration_ms));

        for (float r = start_angle; r <= stop_angle; r += PI / 360)
        {
            float ax = screen_radius + (screen_radius - 8) * cosf(r);
            float ay = screen_radius - (screen_radius - 8) * sinf(r);

            float bx = screen_radius + (screen_radius)*cosf(r);
            float by = screen_radius - (screen_radius)*sinf(r);

            spr_->drawWideLine(ax, ay, bx, by, 2, work_color, work_color);
        }
    }

    if (state == PomodoroState::WORK && remaining_ms <= 0)
    {
        state = PomodoroState::BREAK;
        state_changed = true;
        start_time = now;
    }
    else if (state == PomodoroState::BREAK && remaining_ms <= 0)
    {
        state = PomodoroState::WORK;
        state_changed = true;
        start_time = now;
    }

    char buf_[16];

    if (state == PomodoroState::IDLE)
    {
        spr_->setTextColor(TFT_WHITE);
        spr_->drawString("Press to start", screen_radius, text_begin);
    }
    else if (state == PomodoroState::WORK)
    {
        spr_->setTextColor(TFT_WHITE);
        spr_->drawString("Work!", screen_radius, text_begin);

        spr_->drawString("Time Left: ", screen_radius, text_begin + font_height);

        spr_->setTextDatum(CR_DATUM);
        sprintf(buf_, "%02d:%02d", minutes_remaining, seconds_remaining);
        spr_->drawString(buf_, screen_radius + spr_->textWidth("00:00") / 2, text_begin + font_height * 3);
    }
    else
    {
        spr_->setTextColor(TFT_WHITE);

        spr_->drawString("Break!", screen_radius, text_begin);
        spr_->setTextDatum(CR_DATUM);
        sprintf(buf_, "%02d:%02d", minutes_remaining, seconds_remaining);
        spr_->drawString(buf_, screen_radius + spr_->textWidth("00:00") / 2, text_begin + font_height * 2);
    }

    return spr_;
}