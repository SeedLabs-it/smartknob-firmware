#pragma once
#include "../app.h"
#include "../../font/NDS1210pt7b.h"
#include "../../font/Pixel62mr11pt7b.h"
#include "../../util.h"

const uint8_t LIGHT_DIMMER_APP_MODE_DIMMER = 0;
const uint8_t LIGHT_DIMMER_APP_MODE_HUE = 1;

class LightDimmerApp : public App
{
public:
    LightDimmerApp(TFT_eSprite *spr_, char *app_id, char *friendly_name, char *entity_id);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);
    void updateStateFromSystem(AppState state);

protected:
    int8_t navigationNext();
    int8_t navigationBack();
    TFT_eSprite *renderHUEWheel();

private:
    int16_t current_position = 0;
    int16_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;

    cJSON *json;

    // app state
    uint8_t app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;

    uint16_t app_hue_position = 0;
    uint8_t current_brightness = 0;
    bool is_on = false;

    bool first_run = false;
};