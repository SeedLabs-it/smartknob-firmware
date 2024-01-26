#pragma once
#include "../app.h"
#include "../../font/NDS1210pt7b.h"
#include "../../font/Pixel62mr11pt7b.h"

class LightDimmerApp : public App
{
public:
    LightDimmerApp(TFT_eSprite *spr_, char *app_id, char *friendly_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

private:
    uint16_t current_position = 0;
    uint16_t last_position = 0;
    uint8_t num_positions = 0;
    char *app_id;
    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
};