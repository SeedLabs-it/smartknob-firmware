#pragma once
#include "../app.h"

#include "../../font/roboto_thin_20.h"
#include "../../font/roboto_light_60.h"

class BlindsApp : public App
{
public:
    BlindsApp(TFT_eSprite *spr_, char *app_id, char *friendly_name);
    TFT_eSprite *render();
    int8_t navigationNext();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);
    void updateStateFromSystem(AppState state);

private:
    uint8_t current_closed_position = 0;
    uint8_t last_closed_position = 0;
    char buf_[24];
};