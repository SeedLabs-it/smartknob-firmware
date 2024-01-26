#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"
#include "../../font/roboto_light_60.h"

class StopwatchApp : public App
{
public:
    StopwatchApp(TFT_eSprite *spr_, std::string entity_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

private:
    std::string entity_name;
    unsigned long start_ms;
    bool started = false;

    uint16_t current_position = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
};