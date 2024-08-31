#pragma once
#include "../app.h"
#include "../../util.h"

const uint8_t LIGHT_DIMMER_APP_MODE_DIMMER = 0;
const uint8_t LIGHT_DIMMER_APP_MODE_HUE = 1;

class LightDimmerApp : public App
{
public:
    LightDimmerApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) override;
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update) override;

    int8_t navigationNext() override;
    int8_t navigationBack() override;

private:
    void initScreen() override
    {
        initDimmerScreen();
        initHueScreen();
        updateHueWheel();
    }

    void initDimmerScreen();
    void initHueScreen();

    void updateHueWheel();

    lv_obj_t *arc_;
    lv_obj_t *outer_mask_arc;
    lv_obj_t *inner_mask_circle;
    lv_obj_t *selector_inner_mask_circle;
    lv_obj_t *inner_indicator_mask_arc;

    lv_obj_t *percentage_label_;

    int16_t current_position = 0;
    int16_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;

    cJSON *json;

    // app state
    uint8_t app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;

    bool color_set = false;

    uint8_t app_hue_position = 0;
    uint8_t current_brightness = 0;
    bool is_on = false;

    bool first_run = false;

    int8_t calculateAppHuePosition(int8_t position);

    lv_obj_t *mask_img;
    lv_obj_t *dimmer_screen;
    lv_obj_t *hue_screen;
    lv_obj_t *hue_wheel;

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    lv_color_hsv_t hsv;
};