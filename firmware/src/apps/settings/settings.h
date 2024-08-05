#pragma once
#include "../app.h"
#include "../util.h"
#include "./display/page_manager.h"
#include "pages/wifi.h"
#include "pages/demo.h"
#include "pages/motor_calib.h"
#include "./notify/os_config_notifier/os_config_notifier.h"

enum SettingsPages
{
    WIFI_PAGE_SETTINGS,
    // HASS_PAGE_SETTINGS,
    DEMO_PAGE_SETTINGS,
    MOTOR_CALIBRATION_SETTINGS,
    // STRAIN_CALIBRATION_SETTINGS,
    SETTINGS_PAGE_COUNT
};

class SettingsPageManager : public PageManager<SettingsPages>
{

public:
    SettingsPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex, OSConfigNotifier *os_config_notifier) : PageManager<SettingsPages>(parent, mutex)
    {
        add(WIFI_PAGE_SETTINGS, new WiFiSettingsPage(parent));
        // add(HASS_PAGE_SETTINGS, new HASSSettingsPage(parent));
        DemoSettingsPage *demo_page = new DemoSettingsPage(parent);
        demo_page->setOSConfigNotifier(os_config_notifier);
        add(DEMO_PAGE_SETTINGS, demo_page);
        add(MOTOR_CALIBRATION_SETTINGS, new MotorCalibrationSettingsPage(parent));
        // add(STRAIN_CALIBRATION_SETTINGS, new StrainCalibrationSettingsPage(parent));

        dotIndicatorInit();

        page_name = lv_label_create(overlay_);
        lv_obj_align(page_name, LV_ALIGN_TOP_MID, 0, 10);

        show(WIFI_PAGE_SETTINGS);
    }

    void show(SettingsPages page_enum) override
    {
        PageManager::show(page_enum);
        {
            SemaphoreGuard lock(mutex_);

            for (uint16_t i = 0; i < SETTINGS_PAGE_COUNT; i++)
            {
                lv_obj_t *dot = dots[i];
                if (i == current_page_)
                {
                    lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0xD9, 0xD9, 0xD9), 0);

                    switch (current_page_)
                    {
                    case WIFI_PAGE_SETTINGS:
                        lv_label_set_text(page_name, "WiFi");
                        break;
                    // case HASS_PAGE_SETTINGS:
                    //     lv_label_set_text(page_name, "HASS");
                    //     break;
                    case DEMO_PAGE_SETTINGS:
                        lv_label_set_text(page_name, "DEMO");
                        break;
                    case MOTOR_CALIBRATION_SETTINGS:
                        lv_label_set_text(page_name, "MOTOR");
                        break;
                    // case STRAIN_CALIBRATION_SETTINGS:
                    //     lv_label_set_text(page_name, "STRAIN");
                    //     break;
                    default:
                        lv_label_set_text(page_name, "Unknown");
                        break;
                    }
                }
                else
                {
                    lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0x72, 0x72, 0x72), 0);
                }
            }
        }
    }

    void dotIndicatorInit()
    {
        overlay_ = lv_obj_create(parent_);
        lv_obj_remove_style_all(overlay_);
        lv_obj_set_size(overlay_, LV_HOR_RES, LV_VER_RES);

        for (uint16_t i = 0; i < SETTINGS_PAGE_COUNT; i++)
        {
            lv_obj_t *dot = lvDrawCircle(dot_dia, overlay_);
            lv_obj_align(dot, LV_ALIGN_CENTER, -position_circle_radius * cosf(dot_starting_angle + degree_per_dot * i), -position_circle_radius * sinf(dot_starting_angle + degree_per_dot * i));
            lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0x72, 0x72, 0x72), 0);
            dots[i] = dot;
        }
    }

private:
    lv_obj_t *dots[SETTINGS_PAGE_COUNT];
    lv_obj_t *page_name;

    const uint8_t dot_dia = 6;
    const uint8_t position_circle_radius = LV_HOR_RES / 2 - dot_dia; // the radius of the circle where you want the dots to lay.
    float degree_per_dot = dot_dia * PI / 180;                       // the degree (angle) between two points in radian
    float center_point_degree = (270 - 90) * PI / 180;               //
    float dot_starting_angle = center_point_degree - (((SETTINGS_PAGE_COUNT - 1) * degree_per_dot) / 2);
};

class SettingsApp : public App
{
public:
    SettingsApp(SemaphoreHandle_t mutex);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    void handleNavigation(NavigationEvent event);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);

private:
    ConnectivityState connectivity_state;
    char ip_address[20];
    char ssid[128];

    uint8_t current_position = 0;
    uint8_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
    float old_adjusted_sub_position = 0;
    bool first_run = false;

    SettingsPageManager *page_mgr = nullptr;

    OSConfigNotifier *os_config_notifier_ = nullptr;
};