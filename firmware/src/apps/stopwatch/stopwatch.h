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

    lv_obj_t *relative_time_label;
    lv_obj_t *time_label;
    lv_obj_t *ms_label;

    lv_obj_t *lap_time_label;

    lv_obj_t *start_stop_indicator;
    lv_obj_t *start_stop_label;
};

class StopwatchApp : public App
{
public:
    StopwatchApp(SemaphoreHandle_t mutex, char *entitiy_id);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    int8_t navigationNext();

private:
    void initScreen();

    CurrentStopwatchState current_stopwatch_state;

    bool started = false;

    uint16_t current_position = 0;

    // needed for UI
    char buf_[24];
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
    // TODO: move to a shared const
    const uint8_t laps_max = 100;
    uint8_t last_lap_added = 0;
    LapTime laps[100];

    void timer_task(lv_timer_t *timer);
    void clear();
};