#pragma once
#include "../app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"

class LightSwitchApp : public App
{
public:
    LightSwitchApp(TFT_eSprite *spr_, std::string entity_name, std::string friendly_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

private:
    uint16_t current_position = 0;
    uint16_t last_position = 0;
    uint8_t num_positions = 0;
    std::string entity_name;
    std::string friendly_name;
    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
};