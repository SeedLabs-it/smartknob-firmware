#pragma once
#include "../app.h"
#include "../util.h"
class SwitchApp : public App
{
public:
    SwitchApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id, bool is_light_switch);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);
    void updateStateFromSystem(AppState state);

protected:
    void initScreen();

private:
    lv_img_dsc_t big_icon_active;

    lv_obj_t *arc_;
    lv_obj_t *status_label;

    uint8_t current_position = 0;
    uint8_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
    float old_adjusted_sub_position = 0;
    long last_updated_ms = 0;
    bool first_run = false;

    bool is_light_switch = false;
};