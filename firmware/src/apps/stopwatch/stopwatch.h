#pragma once
#include "../app.h"

struct LapTime
{
    uint8_t m = 0;
    uint8_t s = 0;
    uint8_t ms = 0;          // actually it is 10s of ms
    uint32_t raw_ms = 0;     // time since started
    uint32_t lap_ms = 0;     // time for this lap
    int32_t improvement = 0; // diff with previus lap
};

struct CurrentStopwatchState
{
    unsigned long start_ms;

    lv_obj_t *lap_time_label;
    lv_obj_t *time_label;
    lv_obj_t *ms_label;

    lv_obj_t *start_stop_indicator;
};

class StopwatchApp : public App
{
public:
    StopwatchApp(SemaphoreHandle_t mutex, char *entitiy_id);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    int8_t navigationNext();

private:
    void initScreen()
    {
        SemaphoreGuard lock(mutex_);

        current_stopwatch_state.start_stop_indicator = lv_bar_create(screen);
        lv_obj_t *start_stop_indicator = current_stopwatch_state.start_stop_indicator;
        lv_obj_set_size(start_stop_indicator, 240, 260);
        lv_obj_set_style_bg_opa(start_stop_indicator, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_bg_color(start_stop_indicator, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_PART_INDICATOR);
        lv_obj_set_style_radius(start_stop_indicator, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(start_stop_indicator, 0, LV_PART_INDICATOR);
        lv_obj_center(start_stop_indicator);
        lv_bar_set_value(start_stop_indicator, 0, LV_ANIM_OFF);

        current_stopwatch_state.lap_time_label = lv_label_create(screen);
        lv_obj_t *lap_time_label = current_stopwatch_state.lap_time_label;

        current_stopwatch_state.time_label = lv_label_create(screen);
        current_stopwatch_state.ms_label = lv_label_create(screen);

        lv_obj_t *time_label = current_stopwatch_state.time_label;
        lv_obj_t *ms_label = current_stopwatch_state.ms_label;

        lv_label_set_text(time_label, "00:00.");
        lv_obj_set_style_text_font(time_label, &NDS12_20px, 0);
        lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

        lv_label_set_text(ms_label, "00");
        lv_obj_set_style_text_font(ms_label, &NDS12_10px, 0);
        lv_obj_align_to(ms_label, time_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
    }

    CurrentStopwatchState current_stopwatch_state;

    bool started = false;

    uint16_t current_position = 0;

    // needed for UI
    char buf_[24];
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
    // TODO: move to a shared const
    const uint8_t laps_max = 20;
    uint8_t last_lap_added = 0;
    LapTime laps[20];

    void timer_task(lv_timer_t *timer);
    void clear();
};