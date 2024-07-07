#include "stopwatch.h"

void my_timer(lv_timer_t *timer)
{
    /*Use the user_data*/
    CurrentStopwatchState *user_data = (CurrentStopwatchState *)timer->user_data;
    unsigned long now = millis();
    unsigned long diff_ms = now - user_data->start_ms;
    unsigned long stopwatch_ms = 0;
    unsigned long stopwatch_sec = 0;
    unsigned long stopwatch_min = 0;

    stopwatch_ms = diff_ms % 100;
    stopwatch_sec = floor((diff_ms / 1000) % 60);
    stopwatch_min = floor((diff_ms / (1000 * 60)) % 60);

    lv_label_set_text_fmt(user_data->time_label, "%02d:%02d.", stopwatch_min, stopwatch_sec);
    lv_label_set_text_fmt(user_data->ms_label, "%02d", stopwatch_ms);
}

StopwatchApp::StopwatchApp(SemaphoreHandle_t mutex, char *entitiy_id) : App(mutex)
{
    sprintf(app_id, "%s", "stopwatch");
    sprintf(entitiy_id, "%s", entitiy_id);
    sprintf(friendly_name, "%s", "Stopwatch");

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        4,
        0,
        0,
        60 * PI / 180,
        0.01,
        0.6,
        1.1,
        "",
        0,
        {},
        0,
        45,
    };
    strncpy(motor_config.id, "stopwatch", sizeof(motor_config.id) - 1);

    initScreen();
}

void StopwatchApp::clear()
{
    for (int i = 0; i < laps_max; i++)
    {
        laps[i] = LapTime{};
    }
    last_lap_added = 0;
}

EntityStateUpdate StopwatchApp::updateStateFromKnob(PB_SmartKnobState state)
{
    static lv_timer_t *timer;
    current_position = state.current_position;
    sub_position_unit = state.sub_position_unit;

    // needed to next reload of App
    motor_config.position_nonce = state.current_position;
    motor_config.position = state.current_position;

    EntityStateUpdate new_state;

    lv_obj_t *start_stop_indicator = current_stopwatch_state.start_stop_indicator;
    {
        SemaphoreGuard lock(mutex_);
        if (sub_position_unit >= 0)
        {
            lv_obj_set_style_bg_color(start_stop_indicator, LV_COLOR_MAKE(0x00, 0xFF, 0x00), LV_PART_INDICATOR);
            if (!started && lv_label_get_text(current_stopwatch_state.start_stop_label) != "START")
            {
                lv_label_set_text(current_stopwatch_state.start_stop_label, "START");
            }
            else if (lv_label_get_text(current_stopwatch_state.start_stop_label) != "STARTED")
            {
                lv_label_set_text(current_stopwatch_state.start_stop_label, "STARTED");
            }
        }
        else if (sub_position_unit < 0)
        {
            lv_obj_set_style_bg_color(start_stop_indicator, LV_COLOR_MAKE(0xFF, 0x00, 0x00), LV_PART_INDICATOR);
            if (lv_label_get_text(current_stopwatch_state.start_stop_label) != "RESET")
            {
                lv_label_set_text(current_stopwatch_state.start_stop_label, "RESET");
            }
        }

        if (sub_position_unit <= 1.5 && sub_position_unit >= -1.5)
        {
            lv_bar_set_value(start_stop_indicator, abs(sub_position_unit) / 1.5 * 25, LV_ANIM_OFF);
        }
    }

    if (started && sub_position_unit < -1.5)
    {
        started = false;
        lv_timer_del(timer);

        new_state.play_haptic = true;
    }

    if (!started && sub_position_unit > 1.5)
    {

        started = true;
        // lv_label_set_text(current_stopwatch_state.start_stop_label, "STARTED");
        current_stopwatch_state.start_ms = millis();
        clear();
        timer = lv_timer_create(my_timer, 25, &current_stopwatch_state);

        new_state.play_haptic = true;
    }

    return new_state;
}

void StopwatchApp::updateStateFromSystem(AppState state) {}

int8_t StopwatchApp::navigationNext()
{
    if (last_lap_added >= laps_max)
    {
        return DONT_NAVIGATE;
    }

    if (started)
    {
        unsigned long now = millis();
        uint32_t diff_ms = now - current_stopwatch_state.start_ms; // diff will not be that big ever

        uint32_t lap_ms = diff_ms;

        if (last_lap_added > 0)
        {
            lap_ms = diff_ms - laps[last_lap_added - 1].raw_ms;
        }

        uint32_t stopwatch_ms = 0;
        uint32_t stopwatch_sec = 0;
        uint32_t stopwatch_min = 0;

        stopwatch_ms = lap_ms % 100;
        stopwatch_sec = floor((lap_ms / 1000) % 60);
        stopwatch_min = floor((lap_ms / (1000 * 60)) % 60);

        laps[last_lap_added].m = stopwatch_min;
        laps[last_lap_added].s = stopwatch_sec;
        laps[last_lap_added].ms = stopwatch_ms;
        laps[last_lap_added].raw_ms = diff_ms;
        laps[last_lap_added].lap_ms = lap_ms;

        if (last_lap_added > 0)
        {
            laps[last_lap_added].improvement = int32_t(laps[last_lap_added].lap_ms) - int32_t(laps[last_lap_added - 1].lap_ms);
        }

        last_lap_added++;

        {
            // SemaphoreGuard lock(mutex_);
            // lv_label_set_text_fmt(current_stopwatch_state.lap_time_label, "%02d:%02d.%02d", stopwatch_min, stopwatch_sec, stopwatch_ms);
        }
    }
    // LOGE("Stopwatch navigationNext");

    return DONT_NAVIGATE;
}