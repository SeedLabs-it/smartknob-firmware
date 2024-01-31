#include "pomodoro.h"

// Duration of a Pomodoro in minutes
const long POMODORO_DURATION = 25;

// Duration of a break in minutes
const int BREAK_DURATION = 5;

PomodoroApp::PomodoroApp(TFT_eSprite *spr_) : App(spr_), state(PomodoroState::IDLE), startTime(millis())
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
        startTime = millis();
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
    long elapsed = now - startTime;
    long total_seconds = elapsed / 1000;
    long minutes = total_seconds / 60;

    if (state == PomodoroState::WORK && minutes >= POMODORO_DURATION)
    {
        state = PomodoroState::BREAK;
        startTime = now;
    }
    else if (state == PomodoroState::BREAK && minutes >= BREAK_DURATION)
    {
        state = PomodoroState::WORK;
        startTime = now;
    }

    spr_->fillSprite(TFT_BLACK);

    if (state == PomodoroState::IDLE)
    {
        spr_->setTextColor(TFT_WHITE);
        spr_->drawString("Press to start", TFT_WIDTH / 2, TFT_HEIGHT / 2);
    }
    else if (state == PomodoroState::WORK)
    {
        long elapsed = millis() - startTime;
        long remaining = POMODORO_DURATION * 60 * 1000 - elapsed;
        long total_seconds = remaining / 1000;
        long minutes = total_seconds / 60;
        long seconds = total_seconds % 60;

        spr_->setTextColor(TFT_GREEN);
        spr_->drawString("Work!", TFT_WIDTH / 2, TFT_HEIGHT / 2);

        spr_->drawString("Time Left: ", TFT_WIDTH / 2, TFT_HEIGHT / 2 + 20);
        std::string time_str = std::to_string(minutes) + ":" + std::to_string(seconds);
        spr_->drawString(time_str.c_str(), TFT_WIDTH / 2, TFT_HEIGHT / 2 + 40);
        // SEND STATE PLAY HAPTIC !!!!
    }
    else
    {
        long elapsed = millis() - startTime;
        long remaining = BREAK_DURATION * 60 * 1000 - elapsed;
        long total_seconds = remaining / 1000;
        long minutes = total_seconds / 60;
        long seconds = total_seconds % 60;

        spr_->setTextColor(TFT_RED);

        spr_->drawString("Break!", TFT_WIDTH / 2, TFT_HEIGHT / 2);
        std::string time_str = std::to_string(minutes) + ":" + std::to_string(seconds);
        spr_->drawString(time_str.c_str(), TFT_WIDTH / 2, TFT_HEIGHT / 2 + 40);
    }

    return spr_;
}