#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"
#include "../../font/roboto_light_60.h"

struct LapTime
{
    uint8_t m = 0;
    uint8_t s = 0;
    uint8_t ms = 0;          // actually it is 10s of ms
    uint32_t raw_ms = 0;     // time since started
    uint32_t lap_ms = 0;     // time for this lap
    int32_t improvement = 0; // diff with previus lap
};

class StopwatchApp : public App
{
public:
    StopwatchApp(TFT_eSprite *spr_, char *entitiy_id);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

protected:
    int8_t navigationNext();

private:
    unsigned long start_ms;
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

    void clear();
};