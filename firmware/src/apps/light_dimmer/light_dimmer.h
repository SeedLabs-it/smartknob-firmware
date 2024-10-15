#pragma once
#include "../app.h"
#include "../../util.h"
#include "./display/page_manager.h"

#include "pages/dimmer.h"
#include "pages/page_selector.h"
#include "pages/hue.h"
#include "pages/temp.h"

#define skip_degrees_selectable 4 // TODO should only be in hue.h

enum LightDimmerPages
{
    LIGHT_DIMMER_PAGE = 0,
    PAGE_SELECTOR,
    HUE_PAGE,
    TEMP_PAGE,
    LIGHT_DIMMER_PAGE_COUNT
};

class LightDimmerPageManager : public PageManager<LightDimmerPages>
{
public:
    LightDimmerPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex, const AppData &app_data) : PageManager<LightDimmerPages>(parent, mutex)
    {
        // add(WIFI_PAGE_SETTINGS, new WiFiSettingsPage(parent));
        // // add(HASS_PAGE_SETTINGS, new HASSSettingsPage(parent));
        // DemoSettingsPage *demo_page = new DemoSettingsPage(parent);
        // demo_page->setOSConfigNotifier(os_config_notifier);
        // add(DEMO_PAGE_SETTINGS, demo_page);
        // add(MOTOR_CALIBRATION_SETTINGS, new MotorCalibrationSettingsPage(parent));
        // add(UPDATE_PAGE_SETTINGS, new UpdateSettingsPage(parent));
        // // add(STRAIN_CALIBRATION_SETTINGS, new StrainCalibrationSettingsPage(parent));

        // dotIndicatorInit();

        // page_name = lv_label_create(overlay_);
        // lv_obj_align(page_name, LV_ALIGN_TOP_MID, 0, 10);
        add(LIGHT_DIMMER_PAGE, new DimmerPage(parent, app_data));
        add(PAGE_SELECTOR, new PageSelector(parent));
        add(HUE_PAGE, new HuePage(parent));
        add(TEMP_PAGE, new TempPage(parent));

        show(LIGHT_DIMMER_PAGE);
    }
};

class LightDimmerApp : public App
{
public:
    LightDimmerApp(SemaphoreHandle_t mutex, AppData app_data);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) override;
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update) override;

    int8_t navigationNext() override;
    int8_t navigationBack() override;

    void handleNavigation(NavigationEvent event) override;

private:
    PB_SmartKnobConfig dimmer_config = PB_SmartKnobConfig{
        .position = 0,
        .sub_position_unit = 0,
        .position_nonce = 0,
        .min_position = 0,
        .max_position = 100,
        .position_width_radians = 2.4 * PI / 180,
        .detent_strength_unit = 1,
        .endstop_strength_unit = 1,
        .snap_point = 1.1,
        .detent_positions_count = 0,
        .detent_positions = {},
        .snap_point_bias = 0,
        .led_hue = 27,
    };

    PB_SmartKnobConfig page_selector_config = PB_SmartKnobConfig{
        .position = 0,
        .sub_position_unit = 0,
        .position_nonce = 0,
        .min_position = 0,
        .max_position = LIGHT_DIMMER_PAGE_COUNT - 2,
        .position_width_radians = 25 * PI / 180,
        .detent_strength_unit = 1,
        .endstop_strength_unit = 1,
        .snap_point = 1.1,
        .detent_positions_count = 0,
        .detent_positions = {},
        .snap_point_bias = 0,
        .led_hue = 27,
    };

    PB_SmartKnobConfig hue_config = PB_SmartKnobConfig{
        .position = 0,
        .sub_position_unit = 0,
        .position_nonce = 0,
        .min_position = 0,
        .max_position = -1,
        .position_width_radians = skip_degrees_selectable * PI / 180,
        .detent_strength_unit = 1,
        .endstop_strength_unit = 1,
        .snap_point = 1,
        .detent_positions_count = 0,
        .detent_positions = {},
        .snap_point_bias = 0,
        .led_hue = 27,
    };

    PB_SmartKnobConfig temp_config = PB_SmartKnobConfig{
        .position = 0,
        .sub_position_unit = 0,
        .position_nonce = 0,
        .min_position = 0,
        .max_position = -1,
        .position_width_radians = skip_degrees_selectable * PI / 180,
        .detent_strength_unit = 1,
        .endstop_strength_unit = 1,
        .snap_point = 1,
        .detent_positions_count = 0,
        .detent_positions = {},
        .snap_point_bias = 0,
        .led_hue = 27,
    };

    // void initScreen() override
    // {
    //     // initDimmerScreen();
    //     // initHueScreen();
    //     // updateHueWheel();
    // }

    // void initDimmerScreen();
    // void initHueScreen();

    // void updateHueWheel();

    lv_obj_t *arc_;
    // lv_obj_t *outer_mask_arc;
    // lv_obj_t *inner_mask_circle;
    // lv_obj_t *selector_inner_mask_circle;
    // lv_obj_t *inner_indicator_mask_arc;

    lv_obj_t *percentage_label_;

    int16_t current_position = 0;
    int16_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;

    cJSON *json;

    bool color_set = false;

    uint8_t app_hue_position = 0;
    bool is_on = false;

    bool first_run = false;

    int8_t calculateAppHuePosition(int8_t position);

    // lv_obj_t *mask_img;
    lv_obj_t *dimmer_screen;
    // lv_obj_t *hue_screen;
    // lv_obj_t *hue_wheel;

    // lv_obj_t *selected_hue;

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    lv_color_hsv_t hsv;

    LightDimmerPageManager *page_mgr_ = nullptr;
};