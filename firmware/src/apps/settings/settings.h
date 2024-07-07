#pragma once
#include "../app.h"
#include "../util.h"
#include "./display/page_manager.h"
#include "pages/wifi.h"

enum SettingsPages
{
    WIFI_PAGE_SETTINGS,
    HASS_PAGE_SETTINGS,
    DEMO_PAGE_SETTINGS,
    MOTOR_CALIBRATION_SETTINGS,
    STRAIN_CALIBRATION_SETTINGS,
    SETTINGS_PAGE_COUNT
};

class HASSSettingsPage : public BasePage
{
public:
    HASSSettingsPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "Home Assistant Settings");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }
};

class DemoSettingsPage : public BasePage
{
public:
    DemoSettingsPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "Demo Mode");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }
};

class MotorCalibrationSettingsPage : public BasePage
{
public:
    MotorCalibrationSettingsPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "Motor Calibration");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }
};

class StrainCalibrationSettingsPage : public BasePage
{
public:
    StrainCalibrationSettingsPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "Strain Gauge Calibration");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }
};

class SettingsPageManager : public PageManager<SettingsPages>
{
public:
    SettingsPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex) : PageManager<SettingsPages>(parent, mutex)
    {
        add(WIFI_PAGE_SETTINGS, new WiFiSettingsPage(parent));
        add(HASS_PAGE_SETTINGS, new HASSSettingsPage(parent));
        add(DEMO_PAGE_SETTINGS, new DemoSettingsPage(parent));
        add(MOTOR_CALIBRATION_SETTINGS, new MotorCalibrationSettingsPage(parent));
        add(STRAIN_CALIBRATION_SETTINGS, new StrainCalibrationSettingsPage(parent));

        dotIndicatorInit();
        show(WIFI_PAGE_SETTINGS);
    }

    void show(SettingsPages page_enum) override
    {
        PageManager::show(page_enum);
        {
            SemaphoreGuard lock(mutex_);

            for (uint16_t i = 0; i < SETTINGS_PAGE_COUNT; i++)
            {
                if (i == current_page_)
                {
                    lv_obj_set_style_bg_color(dots[i], LV_COLOR_MAKE(0xD9, 0xD9, 0xD9), 0);
                }
                else
                {
                    lv_obj_set_style_bg_color(dots[i], LV_COLOR_MAKE(0x72, 0x72, 0x72), 0);
                }
            }
        }
    }

    void dotIndicatorInit()
    {
        overlay_ = lv_obj_create(parent_);
        lv_obj_remove_style_all(overlay_);
        lv_obj_set_size(overlay_, LV_HOR_RES, LV_VER_RES);

        const uint8_t dot_dia = 12;
        const uint8_t position_circle_radius = LV_HOR_RES / 2 - dot_dia; // the radius of the circle where you want the dots to lay.
        float degree_per_dot = 9 * PI / 180;                             // the degree (angle) between two points in radian
        float center_point_degree = 270 * PI / 180;                      //
        float dot_starting_angle = center_point_degree - (((SETTINGS_PAGE_COUNT - 1) * degree_per_dot) / 2);

        for (uint16_t i = 0; i < SETTINGS_PAGE_COUNT; i++)
        {
            lv_obj_t *dot = lvDrawCircle(12, overlay_);
            lv_obj_align(dot, LV_ALIGN_CENTER, -position_circle_radius * cosf(dot_starting_angle + degree_per_dot * i), -position_circle_radius * sinf(dot_starting_angle + degree_per_dot * i));
            lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0x72, 0x72, 0x72), 0);
            dots[i] = dot;
        }
    }

private:
    lv_obj_t *dots[SETTINGS_PAGE_COUNT];
};

class SettingsApp : public App
{
public:
    SettingsApp(SemaphoreHandle_t mutex);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);

protected:
    void initScreen()
    {
        SemaphoreGuard lock(mutex_);

        lv_obj_t *label = lv_label_create(screen);
        lv_label_set_text(label, "Settings");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 50);

        // arc_ = lv_arc_create(screen);
        // lv_obj_set_size(arc_, 210, 210);
        // lv_arc_set_rotation(arc_, 225);
        // lv_arc_set_bg_angles(arc_, 0, 90);
        // // lv_arc_set_knob_offset(arc_, 0);
        // lv_arc_set_value(arc_, 0);
        // lv_obj_center(arc_);

        // lv_obj_set_style_arc_opa(arc_, LV_OPA_0, LV_PART_MAIN);
        // lv_obj_set_style_arc_opa(arc_, LV_OPA_0, LV_PART_INDICATOR);
        // lv_obj_set_style_bg_color(arc_, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_KNOB);

        // lv_obj_set_style_arc_width(arc_, 1, LV_PART_INDICATOR);

        // light_bulb = lv_img_create(screen);
        // lv_img_set_src(light_bulb, &big_icon);
        // // lv_obj_set_style_image_recolor_opa(light_bulb, LV_OPA_COVER, 0);
        // lv_obj_set_style_img_recolor_opa(light_bulb, LV_OPA_COVER, 0);
        // // lv_obj_set_style_image_recolor(light_bulb, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), 0);
        // lv_obj_set_style_img_recolor(light_bulb, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), 0);

        // lv_obj_center(light_bulb);

        // lv_obj_t *label = lv_label_create(screen);
        // lv_label_set_text(label, friendly_name);
        // lv_obj_align_to(label, light_bulb, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    };

private:
    // lv_img_dsc_t big_icon_active;

    // lv_obj_t *;
    // lv_obj_t *light_bulb;

    uint8_t current_position = 0;
    uint8_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
    float old_adjusted_sub_position = 0;
    bool first_run = false;

    SettingsPageManager *page_mgr = nullptr;
};