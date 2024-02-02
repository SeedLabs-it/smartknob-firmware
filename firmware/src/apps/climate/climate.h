#pragma once
#include "../app.h"

#include "../../font/NDS1210pt7b.h"
#include "../../font/NDS125_small.h"
#include "../../font/Pixel62mr11pt7b.h"

const uint8_t CLIMATE_APP_MODE_AUTO = 0;
const uint8_t CLIMATE_APP_MODE_COOLING = 1;
const uint8_t CLIMATE_APP_MODE_HEATING = 2;
const uint8_t CLIMATE_APP_MODE_VENTILATION = 3;

const uint8_t CLIMATE_APP_MIN_TEMP = 16;
const uint8_t CLIMATE_APP_MAX_TEMP = 35;

class ClimateApp : public App
{
public:
    ClimateApp(TFT_eSprite *spr_, std::string entity_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

protected:
    int8_t navigationNext();

private:
    // needed for UI
    uint8_t num_positions;
    std::string entity_name;
    float adjusted_sub_position = 0;

    // climate related settings
    uint8_t mode = CLIMATE_APP_MODE_AUTO;
    uint8_t min_temp = CLIMATE_APP_MIN_TEMP;
    uint8_t max_temp = CLIMATE_APP_MAX_TEMP;

    uint8_t current_temperature = 0;
    uint8_t wanted_temperature = 0;
    uint8_t last_wanted_temperature = 0;

    void drawDots();
};