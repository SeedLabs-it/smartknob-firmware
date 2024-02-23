#pragma once
#include "../app.h"
#include "../../font/NDS1210pt7b.h"
#include "../../font/Pixel62mr11pt7b.h"
#include "../../util.h"
#include "../../display_buffer.h"

const uint8_t LIGHT_DIMMER_APP_MODE_DIMMER = 0;
const uint8_t LIGHT_DIMMER_APP_MODE_HUE = 1;

class LightDimmerApp : public App
{
public:
    LightDimmerApp() : App(){};
#ifdef USE_DISPLAY_BUFFER
    LightDimmerApp(char *app_id, std::string entity_name);
    void render();
#else
    LightDimmerApp(TFT_eSprite *spr_) : App(spr_){};
    TFT_eSprite *render();
#endif
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

protected:
    int8_t navigationNext();
    #ifdef USE_DISPLAY_BUFFER
    void renderHUEWheel();
    #else    
    TFT_eSprite *renderHUEWheel();
    #endif

private:
    int16_t current_position = 0;
    int16_t last_position = 0;
    uint8_t num_positions = 0;
    char *app_id;
    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;

    cJSON *json;

    // app state
    uint8_t app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;

    uint16_t app_hue_position = 0;
    uint8_t current_brightness = 0;
};