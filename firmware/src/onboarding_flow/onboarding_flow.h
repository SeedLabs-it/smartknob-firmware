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
    WELCOME_PAGE,
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
        LV_IMAGE_DECLARE(logo_main_gradient_transparent);
        lv_img_set_src(img, &logo_main_gradient_transparent);
        lv_obj_set_width(img, logo_main_gradient_transparent.header.w);
        lv_obj_set_height(img, logo_main_gradient_transparent.header.h);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -54);

        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "SMART KNOB\nDEV KIT v0.1");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 32);
        lv_obj_set_style_text_font(label, &NDS12_20px, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        label = lv_label_create(page);
        lv_label_set_text(label, "ROTATE TO START");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 74);
        lv_obj_set_style_text_font(label, &NDS12_15px, 0);
        lv_obj_set_style_text_color(label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_STATE_DEFAULT);
    }
};

class HassPage : public BasePage
{
public:
    HassPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *img = lv_img_create(page);
        LV_IMAGE_DECLARE(home_assistant_logomark_color_on_light);
        lv_img_set_src(img, &home_assistant_logomark_color_on_light);
        lv_image_set_scale(img, 200);
        lv_obj_set_width(img, home_assistant_logomark_color_on_light.header.w);
        lv_obj_set_height(img, home_assistant_logomark_color_on_light.header.h);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -54);

        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "HOME ASSISTANT\nINTEGRATION");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 32);
        lv_obj_set_style_text_font(label, &NDS12_20px, 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        label = lv_label_create(page);
        lv_label_set_text(label, "PRESS TO CONFIGURE");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 74);
        lv_obj_set_style_text_font(label, &NDS12_15px, 0);
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
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "Demo Mode");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        label = lv_label_create(page);
        lv_label_set_text(label, "PRESS TO START");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 74);
        lv_obj_set_style_text_font(label, &NDS12_15px, 0);
        lv_obj_set_style_text_color(label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_STATE_DEFAULT);
    }
};

class AboutPage : public BasePage
{
public:
    AboutPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "About");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
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
        float dot_starting_angle = center_point_degree - (((ONBOARDING_FLOW_PAGE_COUNT - 1) * degree_per_dot) / 2);

        for (uint16_t i = 0; i < ONBOARDING_FLOW_PAGE_COUNT; i++)
        {
            lv_obj_t *dot = lvDrawCircle(overlay_, 12);
            lv_obj_align(dot, LV_ALIGN_CENTER, -position_circle_radius * cosf(dot_starting_angle + degree_per_dot * i), -position_circle_radius * sinf(dot_starting_angle + degree_per_dot * i));
            lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0x72, 0x72, 0x72), 0);
            dots[i] = dot;
        }
    }

private:
    lv_obj_t *dots[ONBOARDING_FLOW_PAGE_COUNT];
};

class OnboardingFlow

{
public:
    OnboardingFlow(SemaphoreHandle_t mutex);

    void render();

    EntityStateUpdate update(AppState state);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);

    void setWiFiNotifier(WiFiNotifier *wifi_notifier);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void triggerMotorConfigUpdate();
    void handleNavigationEvent(NavigationEvent event);

private:
    SemaphoreHandle_t mutex_;

    HassOnboardingFlow *hass_flow;

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