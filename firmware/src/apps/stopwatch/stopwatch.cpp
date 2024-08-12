#include "stopwatch.h"

void stopwatch_timer(lv_timer_t *timer)
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

    LV_IMG_DECLARE(x80_timer);
    LV_IMG_DECLARE(x40_timer);

    big_icon = x80_timer;
    small_icon = x40_timer;

    initScreen();
}

void StopwatchApp::initScreen()
{
    SemaphoreGuard lock(mutex_);

    current_stopwatch_state.time_label = lv_label_create(screen);
    current_stopwatch_state.ms_label = lv_label_create(screen);

    lv_obj_t *time_label = current_stopwatch_state.time_label;
    lv_obj_t *ms_label = current_stopwatch_state.ms_label;

    current_stopwatch_state.relative_time_label = lv_label_create(screen);
    lv_obj_t *relative_time_label = current_stopwatch_state.relative_time_label;
    lv_obj_align(relative_time_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_label_set_text(relative_time_label, "");
    lv_obj_set_style_text_font(relative_time_label, &roboto_light_mono_16pt, 0);

    lv_label_set_text(time_label, "00:00.");
    lv_obj_set_style_text_font(time_label, &roboto_light_mono_48pt, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, -10, -10);

    lv_label_set_text(ms_label, "00");
    lv_obj_set_style_text_font(ms_label, &roboto_light_mono_24pt, 0);
    lv_obj_align_to(ms_label, time_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -4);

    current_stopwatch_state.lap_time_label = lv_label_create(screen);
    lv_obj_t *lap_time_label = current_stopwatch_state.lap_time_label;
    lv_obj_align_to(lap_time_label, time_label, LV_ALIGN_OUT_BOTTOM_MID, -32, 4);
    lv_label_set_text(lap_time_label, "");
    lv_obj_set_style_text_font(lap_time_label, &roboto_semi_bold_mono_12pt, 0);

    current_stopwatch_state.start_stop_indicator = lv_bar_create(screen);
    lv_obj_t *start_stop_indicator = current_stopwatch_state.start_stop_indicator;
    lv_obj_set_size(start_stop_indicator, 240, 260);
    lv_obj_set_style_bg_opa(start_stop_indicator, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_color(start_stop_indicator, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_PART_INDICATOR);
    lv_obj_set_style_radius(start_stop_indicator, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(start_stop_indicator, 0, LV_PART_INDICATOR);
    lv_obj_center(start_stop_indicator);
    lv_bar_set_value(start_stop_indicator, 0, LV_ANIM_OFF);

    current_stopwatch_state.start_stop_label = lv_label_create(screen);
    lv_obj_t *start_stop_label = current_stopwatch_state.start_stop_label;
    lv_label_set_text(start_stop_label, "START");
    lv_obj_set_style_text_color(start_stop_label, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_PART_MAIN);
    lv_obj_align_to(start_stop_label, start_stop_indicator, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void StopwatchApp::clear()
{
    for (int i = 0; i < laps_max; i++)
    {
        laps[i] = LapTime{};
    }
    last_lap_added = 0;

    lv_label_set_text(current_stopwatch_state.lap_time_label, "");
    lv_label_set_text(current_stopwatch_state.relative_time_label, "");
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

    if (sub_position_unit < 0.1 && sub_position_unit > -0.1)
    {
        if (lv_bar_get_value(current_stopwatch_state.start_stop_indicator) != 0)
        {
            SemaphoreGuard lock(mutex_);
            lv_bar_set_value(current_stopwatch_state.start_stop_indicator, 0, LV_ANIM_OFF);
        }
        return new_state;
    }

    lv_obj_t *start_stop_indicator = current_stopwatch_state.start_stop_indicator;
    {

        if (sub_position_unit > 0.1)
        {
            SemaphoreGuard lock(mutex_);
            lv_obj_set_style_bg_color(start_stop_indicator, LV_COLOR_MAKE(0x00, 0xFF, 0x00), LV_PART_INDICATOR);
            if (!started && lv_label_get_text(current_stopwatch_state.start_stop_label) != "START")
            {
                lv_obj_align_to(current_stopwatch_state.start_stop_label, start_stop_indicator, LV_ALIGN_BOTTOM_MID, 0, -30);
                lv_label_set_text(current_stopwatch_state.start_stop_label, "START");
            }
            else if (lv_label_get_text(current_stopwatch_state.start_stop_label) != "STARTED")
            {
                lv_label_set_text(current_stopwatch_state.start_stop_label, "STARTED");
                lv_obj_align_to(current_stopwatch_state.start_stop_label, start_stop_indicator, LV_ALIGN_BOTTOM_MID, 0, -30);
            }
        }
        else if (sub_position_unit < -0.1)
        {
            SemaphoreGuard lock(mutex_);
            lv_obj_set_style_bg_color(start_stop_indicator, LV_COLOR_MAKE(0xFF, 0x00, 0x00), LV_PART_INDICATOR);
            if (lv_label_get_text(current_stopwatch_state.start_stop_label) != "RESET")
            {
                lv_obj_align_to(current_stopwatch_state.start_stop_label, start_stop_indicator, LV_ALIGN_BOTTOM_MID, 0, -30);
                lv_label_set_text(current_stopwatch_state.start_stop_label, "RESET");
            }
        }

        if (sub_position_unit <= 1.5 && sub_position_unit >= -1.5)
        {
            SemaphoreGuard lock(mutex_);
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
        current_stopwatch_state.start_ms = millis();
        clear();
        timer = lv_timer_create(stopwatch_timer, 25, &current_stopwatch_state);

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
            SemaphoreGuard lock(mutex_);
            char lap_times[256] = "";
            for (int i = max(0, last_lap_added - 6); i < last_lap_added; i++)
            {
                char lap_buffer[64];
                snprintf(lap_buffer, sizeof(lap_buffer), "Lap %02d - %02d:%02d.%02d\n",
                         i + 1, laps[i].m, laps[i].s, laps[i].ms);
                char temp_buffer[256];
                snprintf(temp_buffer, sizeof(temp_buffer), "%s%s", lap_buffer, lap_times);
                strncpy(lap_times, temp_buffer, sizeof(lap_times) - 1);
                lap_times[sizeof(lap_times) - 1] = '\0';
            }

            lv_label_set_text(current_stopwatch_state.lap_time_label, lap_times);

            if (last_lap_added > 1)
            {
                int32_t lap_improvement_ms = abs(laps[last_lap_added - 1].improvement % 100);
                int32_t lap_improvementh_sec = abs(floor((laps[last_lap_added - 1].improvement / 1000) % 60));
                int32_t lap_improvement_min = abs(floor((laps[last_lap_added - 1].improvement / (1000 * 60)) % 60));

                char sign = '+';
                lv_obj_set_style_text_color(current_stopwatch_state.relative_time_label, LV_COLOR_MAKE(0xFF, 0x00, 0x00), 0);
                if (laps[last_lap_added - 1].improvement < 0)
                {
                    lv_obj_set_style_text_color(current_stopwatch_state.relative_time_label, LV_COLOR_MAKE(0x00, 0xFF, 0x00), 0);
                    sign = '-';
                }

                lv_label_set_text_fmt(current_stopwatch_state.relative_time_label, "%c%02d:%02d.%02d", sign, lap_improvement_min, lap_improvementh_sec, lap_improvement_ms);
            }
        }
    }

    return DONT_NAVIGATE;
}