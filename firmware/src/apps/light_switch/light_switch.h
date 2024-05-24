#pragma once
#include "../util.h"
#include "../lvgl_app.h"

#include "../../font/roboto_thin_bold_24.h"
#include "../../font/roboto_thin_20.h"

class LightSwitchApp : public App
{
public:
    LightSwitchApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);
    void updateStateFromSystem(AppState state);

protected:
    void initScreen()
    {
        lv_obj_t *circle = lvDrawCircle(screen, 20);
        lv_obj_set_style_bg_color(circle, LV_COLOR_MAKE(0xFF, 0x00, 0x00), 0);
        lv_obj_align(circle, LV_ALIGN_CENTER, 0, -100);
    };

private:
    uint8_t current_position = 0;
    uint8_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
    bool first_run = false;
};