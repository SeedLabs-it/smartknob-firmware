#pragma once
#include "../app.h"

class BlindsApp : public App
{
public:
    BlindsApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);

private:
    void initScreen();

    lv_obj_t *blinds_bar;
    lv_obj_t *percentage_label;

    uint8_t current_closed_position = 0;
    uint8_t last_closed_position = 0;
    char buf_[24];
};