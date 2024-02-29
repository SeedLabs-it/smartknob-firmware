#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"
#include "../../font/roboto_light_60.h"
#include "../../display_buffer.h"

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
#ifdef USE_DISPLAY_BUFFER
    StopwatchApp(std::string entity_name);
    void render();
#else
    StopwatchApp(TFT_eSprite *spr_, std::string entity_name);
    TFT_eSprite *render();
#endif

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

protected:
    int8_t navigationNext();

private:
    std::string entity_name;
    unsigned long start_ms;
    bool started = false;

    uint32_t backgrounds[5] = {TFT_BLACK, TFT_RED, TFT_BLUE, TFT_PURPLE, TFT_GREEN};
    uint8_t current_background = 0;
    uint16_t current_position = 0;
    uint16_t autoswitchbg_last_sec = 0;

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