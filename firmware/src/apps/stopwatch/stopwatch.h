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
    lv_obj_t *time_label;
    lv_obj_t *ms_label;
};

class StopwatchApp : public App
{
public:
    StopwatchApp(SemaphoreHandle_t mutex, char *entitiy_id);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    // void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);

    // protected:
    //     int8_t navigationNext();

private:
    void initScreen()
    {
        SemaphoreGuard lock(mutex_);

        current_stopwatch_state.time_label = lv_label_create(screen);
        lv_label_set_text(current_stopwatch_state.time_label, "00:00.");
        lv_obj_set_style_text_font(current_stopwatch_state.time_label, &NDS12_20px, 0);
        lv_obj_align(current_stopwatch_state.time_label, LV_ALIGN_CENTER, 0, 0);

        current_stopwatch_state.ms_label = lv_label_create(screen);
        lv_label_set_text(current_stopwatch_state.ms_label, "00");
        lv_obj_set_style_text_font(current_stopwatch_state.ms_label, &NDS12_10px, 0);
        lv_obj_align_to(current_stopwatch_state.ms_label, current_stopwatch_state.time_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);
    }

    // lv_obj_t *time_label;
    // lv_obj_t *ms_label;

    CurrentStopwatchState current_stopwatch_state;

    // unsigned long start_ms;
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