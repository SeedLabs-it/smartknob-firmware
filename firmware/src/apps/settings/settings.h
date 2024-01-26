#pragma once
#include "../app.h"

#include "../../font/NDS1210pt7b.h"
#include "../../font/roboto_thin_20.h"
#include "../../font/roboto_thin_20.h"

class SettingsApp : public App
{
public:
    SettingsApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

private:
    // uint8_t current_volume = 0;
    // uint8_t current_volume_position = 0;

    int32_t current_position = 0;
    uint8_t num_positions;
    ConnectivityState connectivity_state;
    ProximityState proximity_state;
    char room[24];
    char fw_version[24];
    // needed for UI
    float adjusted_sub_position = 0;

    // screen tearing test
    unsigned long startup_ms;
    unsigned long startup_diff_ms;
    uint16_t screen_fps;
};