#pragma once
#include "../app.h"

enum ClimateAppMode
{
    CLIMATE_OFF = 0,
    CLIMATE_HEAT,
    CLIMATE_COOL,
    CLIMATE_HEAT_COOL,
    CLIMATE_AUTO,
    CLIMATE_DRY,
    CLIMATE_FAN_ONLY,
    CLIMATE_MODE_COUNT
};

const uint8_t CLIMATE_APP_MIN_TEMP = 16;
const uint8_t CLIMATE_APP_MAX_TEMP = 35;

const uint16_t MIN_ANGLE = 0;
const uint16_t MAX_ANGLE = 228;
const uint8_t ARC_WIDTH = 16;

class ClimateApp : public App
{
public:
    ClimateApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) override;
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update) override;

    int8_t navigationNext();

private:
    void initScreen();
    void initTemperatureArc();
    void updateTemperatureArc();
    void updateModeIcon();

    // void updateTargetTemperature(uint8_t temperature);

    float adjusted_sub_position = 0;
    bool first_run = false;

    ClimateAppMode mode = ClimateAppMode::CLIMATE_AUTO;
    ClimateAppMode last_mode = ClimateAppMode::CLIMATE_AUTO;

    // app state
    int8_t current_temperature = CLIMATE_APP_MIN_TEMP;
    int8_t target_temperature = CLIMATE_APP_MIN_TEMP;
    int8_t last_target_temperature = CLIMATE_APP_MIN_TEMP;
    // uint8_t current_mode = 0;
    // uint8_t current_fan_mode = 0;
    // bool is_on = false;

    // lv_obj_t *mode_off_icon;
    lv_obj_t *state_label;
    lv_obj_t *target_temp_label;
    lv_obj_t *current_temp_label;

    lv_obj_t *mode_auto_icon;
    lv_obj_t *mode_cool_icon;
    lv_obj_t *mode_heat_icon;
    lv_obj_t *mode_air_icon;

    // lv_obj_t *temperature_meter;
    lv_obj_t *temperature_arc;

    lv_obj_t **temperature_dots;

    const lv_color_t inactive_color = LV_COLOR_MAKE(0x47, 0x47, 0x47);
    const lv_color_t auto_active_color = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF);
    const lv_color_t cool_active_color = LV_COLOR_MAKE(0x50, 0x64, 0xC8);
    const lv_color_t dark_cool_active_color = LV_COLOR_MAKE(0x3E, 0x4E, 0x9C);
    const lv_color_t heat_active_color = LV_COLOR_MAKE(0xFF, 0x80, 0x00);
    const lv_color_t dark_heat_active_color = LV_COLOR_MAKE(0xC7, 0x6D, 0x12);
    const lv_color_t air_active_color = LV_COLOR_MAKE(0xB4, 0xFF, 0x00);
};
