#pragma once
// #include "apps/app.h"
#include <map>
#include "semaphore_guard.h"
#include "./display/page_manager.h"
#include "app_config.h"
#include "../navigation/navigation.h"

// #include "util.h"
// #include "qrcode.h"
// #include "navigation/navigation.h"
// #include "events/events.h"

// // Fonts
// #include "font/roboto_thin_bold_24.h"
// #include "font/roboto_thin_20.h"
// #include "font/NDS125_small.h"
// #include "font/Pixel62mr11pt7b.h"
#include "lvgl.h"
#include "util.h"
#include "logging.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"
#include "notify/os_config_notifier/os_config_notifier.h"

#include "onboarding_flow/submenus/hass_flow.h"
// #include "./assets/images/skdk/logo"

enum OnboardingFlowPages
{
    WELCOME_PAGE = 0,
    HASS_PAGE,
    // WIFI,
    DEMO_PAGE,
    ABOUT_PAGE,
    ONBOARDING_FLOW_PAGE_COUNT
};

class WelcomePage : public BasePage
{
public:
    WelcomePage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *img = lv_img_create(page);
        LV_IMG_DECLARE(logo_main_gradient_transp);
        lv_img_set_src(img, &logo_main_gradient_transp);
        lv_obj_set_width(img, logo_main_gradient_transp.header.w);
        lv_obj_set_height(img, logo_main_gradient_transp.header.h);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -54);

        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "SMART KNOB\nDEV KIT v0.1");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 32);
        lv_obj_set_style_text_font(label, &nds12_20px, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        label = lv_label_create(page);
        lv_label_set_text(label, "ROTATE TO START");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 74);
        lv_obj_set_style_text_font(label, &nds12_14px, 0);
        lv_obj_set_style_text_color(label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_STATE_DEFAULT);
    }
};

class HassPage : public BasePage
{
public:
    HassPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *img = lv_img_create(page);
        LV_IMG_DECLARE(hass_logo_color);
        lv_img_set_src(img, &hass_logo_color);
        // lv_img_set_scale(img, 200);
        // lv_img_set_size_mod
        lv_obj_set_width(img, hass_logo_color.header.w);
        lv_obj_set_height(img, hass_logo_color.header.h);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -54);

        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "HOME ASSISTANT\nINTEGRATION");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 32);
        lv_obj_set_style_text_font(label, &nds12_20px, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        label = lv_label_create(page);
        lv_label_set_text(label, "PRESS TO CONFIGURE");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 74);
        lv_obj_set_style_text_font(label, &nds12_14px, 0);
        lv_obj_set_style_text_color(label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_STATE_DEFAULT);
    }
};

// class WiFiPage : public BasePage
// {
// public:
//     WiFiPage(lv_obj_t *parent) : BasePage(parent)
//     {
//         lv_obj_t *label = lv_label_create(page);
//         lv_label_set_text(label, "WiFi Configuration");
//         lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
//     }
// };

class DemoPage : public BasePage
{
public:
    DemoPage(lv_obj_t *parent) : BasePage(parent)
    {
        uint8_t dot_dia = 24;
        uint8_t distance_between = dot_dia / 2 + 4;
        uint8_t y_offset = 40;

        lv_obj_t *blue_colored_dot = lvDrawCircle(dot_dia, page);
        lv_obj_align(blue_colored_dot, LV_ALIGN_CENTER, 0, -(distance_between * 2) - y_offset);
        lv_obj_set_style_bg_color(blue_colored_dot, LV_COLOR_MAKE(0x28, 0xAD, 0xF7), LV_PART_MAIN);

        lv_obj_t *turquoise_colored_dot = lvDrawCircle(dot_dia, page);
        lv_obj_align(turquoise_colored_dot, LV_ALIGN_CENTER, distance_between * 2, -distance_between - y_offset);
        lv_obj_set_style_bg_color(turquoise_colored_dot, LV_COLOR_MAKE(0x53, 0xC8, 0xBA), LV_PART_MAIN);

        lv_obj_t *green_colored_dot = lvDrawCircle(dot_dia, page);
        lv_obj_align(green_colored_dot, LV_ALIGN_CENTER, distance_between * 2, distance_between - y_offset);
        lv_obj_set_style_bg_color(green_colored_dot, LV_COLOR_MAKE(0x8F, 0xEC, 0x67), LV_PART_MAIN);

        lv_obj_t *yellow_colored_dot = lvDrawCircle(dot_dia, page);
        lv_obj_align(yellow_colored_dot, LV_ALIGN_CENTER, 0, distance_between * 2 - y_offset);
        lv_obj_set_style_bg_color(yellow_colored_dot, LV_COLOR_MAKE(0xEF, 0xEE, 0x00), LV_PART_MAIN);

        lv_obj_t *red_colored_dot = lvDrawCircle(dot_dia, page);
        lv_obj_align(red_colored_dot, LV_ALIGN_CENTER, -(distance_between * 2), distance_between - y_offset);
        lv_obj_set_style_bg_color(red_colored_dot, LV_COLOR_MAKE(0xFE, 0x43, 0x44), LV_PART_MAIN);

        lv_obj_t *purple_colored_dot = lvDrawCircle(dot_dia, page);
        lv_obj_align(purple_colored_dot, LV_ALIGN_CENTER, -(distance_between * 2), -distance_between - y_offset);
        lv_obj_set_style_bg_color(purple_colored_dot, LV_COLOR_MAKE(0x9D, 0x58, 0xF5), LV_PART_MAIN);

        lv_obj_t *page_label = lv_label_create(page);
        lv_label_set_text(page_label, "DEMO");
        lv_obj_align(page_label, LV_ALIGN_CENTER, 0, 32);
        lv_obj_set_style_text_font(page_label, &nds12_20px, 0);
        lv_obj_set_style_text_align(page_label, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_t *prompt_label = lv_label_create(page);
        lv_label_set_text(prompt_label, "PRESS TO START");
        lv_obj_align(prompt_label, LV_ALIGN_CENTER, 0, 74);
        lv_obj_set_style_text_font(prompt_label, &nds12_14px, 0);
        lv_obj_set_style_text_color(prompt_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_STATE_DEFAULT);
    }
};

class AboutPage : public BasePage
{
public:
    AboutPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *url_qr = lv_qrcode_create(page, 80, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_COLOR_MAKE(0x00, 0x00, 0x00));
        lv_qrcode_update(url_qr, "https://store.seedlabs.it/products/smartknob-devkit-v0-1", strlen("https://store.seedlabs.it/products/smartknob-devkit-v0-1"));
        lv_obj_align(url_qr, LV_ALIGN_CENTER, 0, -20);

        lv_obj_t *url_label = lv_label_create(page);
        lv_label_set_text(url_label, "seedlabs.it");
        lv_obj_set_style_text_color(url_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_align_to(url_label, url_qr, LV_ALIGN_OUT_TOP_MID, 0, -2);

        lv_obj_t *hardware_version_label = lv_label_create(page);
        lv_obj_set_style_text_color(hardware_version_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_label_set_text(hardware_version_label, MODEL);
        lv_obj_align_to(hardware_version_label, url_qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);

        lv_obj_t *firmware_version_label = lv_label_create(page);
        lv_label_set_text_fmt(firmware_version_label, "FIRMWARE %s", RELEASE_VERSION);
        // lv_obj_center(firmware_version_label);
        lv_obj_align_to(firmware_version_label, hardware_version_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    }
};

class OnboardingPageManager : public PageManager<OnboardingFlowPages>
{
public:
    OnboardingPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex) : PageManager<OnboardingFlowPages>(parent, mutex)
    {
        add(WELCOME_PAGE, new WelcomePage(parent));
        add(HASS_PAGE, new HassPage(parent));
        add(DEMO_PAGE, new DemoPage(parent));
        add(ABOUT_PAGE, new AboutPage(parent));

        dotIndicatorInit();
        show(WELCOME_PAGE);
    }

    void show(OnboardingFlowPages page_enum) override
    {
        PageManager::show(page_enum);
        {
            SemaphoreGuard lock(mutex_);

            for (uint16_t i = 0; i < ONBOARDING_FLOW_PAGE_COUNT; i++)
            {
                lv_obj_t *dot = dots[i];
                if (i == current_page_)
                {
                    lv_obj_set_size(dot, dot_dia * 1.2, dot_dia * 1.2);
                    lv_obj_align(dot, LV_ALIGN_CENTER, -position_circle_radius * cosf(dot_starting_angle + degree_per_dot * i), -position_circle_radius * sinf(dot_starting_angle + degree_per_dot * i));
                    lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0xD9, 0xD9, 0xD9), 0);
                }
                else
                {
                    lv_obj_set_size(dot, dot_dia, dot_dia);
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

        for (uint16_t i = 0; i < ONBOARDING_FLOW_PAGE_COUNT; i++)
        {
            lv_obj_t *dot = lvDrawCircle(dot_dia, overlay_);
            lv_obj_align(dot, LV_ALIGN_CENTER, -position_circle_radius * cosf(dot_starting_angle + degree_per_dot * i), -position_circle_radius * sinf(dot_starting_angle + degree_per_dot * i));
            lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0x72, 0x72, 0x72), 0);
            dots[i] = dot;
        }
    }

private:
    const uint8_t dot_dia = 6;
    const uint8_t position_circle_radius = LV_HOR_RES / 2 - dot_dia * 1.2; // the radius of the circle where you want the dots to lay.
    const float degree_per_dot = dot_dia * 1.2 * PI / 180;
    const float center_point_degree = (270 - 90) * PI / 180; // 270 is bottom
    const float dot_starting_angle = center_point_degree - (((ONBOARDING_FLOW_PAGE_COUNT - 1) * degree_per_dot) / 2);
    lv_obj_t *dots[ONBOARDING_FLOW_PAGE_COUNT];
};

enum ActiveSubMenu
{
    NONE,
    HASS_SUB_MENU,
    WIFI_SUB_MENU,
    DEMO_SUB_MENU,
    ACTIVE_SUB_MENU_COUNT
};

class OnboardingFlow

{
public:
    OnboardingFlow(SemaphoreHandle_t mutex);

    void render();

    EntityStateUpdate update(AppState state);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);

    void handleEvent(WiFiEvent event);

    void setWiFiNotifier(WiFiNotifier *wifi_notifier);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void triggerMotorConfigUpdate();
    void handleNavigationEvent(NavigationEvent event);

private:
    SemaphoreHandle_t mutex_;

    HassOnboardingFlow *hass_flow;

    ActiveSubMenu active_sub_menu = NONE;

    uint8_t current_position = 0;
    char firmware_version[16];

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config;

    WiFiNotifier *wifi_notifier;
    OSConfigNotifier *os_config_notifier;
    MotorNotifier *motor_notifier;

    void indicatorDots();

    lv_obj_t *overlay;
    lv_obj_t *main_screen = lv_obj_create(NULL);

    OnboardingPageManager *page_mgr = nullptr;
};