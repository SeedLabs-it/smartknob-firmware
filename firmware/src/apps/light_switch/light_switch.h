#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"

#include "../../display_buffer.h"

class LightSwitchApp : public App
{
public:
#ifdef USE_DISPLAY_BUFFER
    LightSwitchApp(char *app_id, char *friendly_name);
    void render();
#else
    LightSwitchApp(TFT_eSprite *spr_, char *app_id, char *friendly_name);
    TFT_eSprite *render();
#endif

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