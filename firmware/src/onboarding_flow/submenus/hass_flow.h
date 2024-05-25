#pragma once
#include "display/page_manager.h"

enum HassOnboardingPages
{
    CONNECT_QRCODE_PAGE = 0,
    WEBSERVER_QRCODE_PAGE,
    CONTINUE_IN_BROWSER_WIFI_PAGE,
    CONNECTING_TO_WIFI_PAGE,
    CONTINUE_IN_BROWSER_MQTT_PAGE,
    CONNECTING_TO_MQTT_PAGE,
    HASS_ONBOARDING_PAGE_COUNT
};

HassOnboardingPages getHassPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= HASS_ONBOARDING_PAGE_COUNT - 1)
    {
        return static_cast<HassOnboardingPages>(screen);
    }
}

class ConnectQRCodePage : public BasePage
{
public:
    ConnectQRCodePage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *img = lv_img_create(page);
        LV_IMAGE_DECLARE(qr_code);
        lv_img_set_src(img, &qr_code);
        lv_obj_set_width(img, qr_code.header.w);
        lv_obj_set_height(img, qr_code.header.h);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    }
};

class HassOnboardingPageManager : public PageManager<HassOnboardingPages>
{
    HassOnboardingPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex) : PageManager<HassOnboardingPages>(parent, mutex)
    {
        add(CONNECT_QRCODE_PAGE, new ConnectQRCodePage(parent));

        show(CONNECT_QRCODE_PAGE);
    }
};

class HassOnboardingFlow
{
public:
    HassOnboardingFlow(SemaphoreHandle_t mutex) : mutex_(mutex)
    {
        root_level_motor_config = PB_SmartKnobConfig{
            0,
            0,
            0,
            0,
            0,
            35 * PI / 180,
            2,
            1,
            0.55,
            "",
            0,
            {},
            0,
            20,
        };

        page_mgr = new HassOnboardingPageManager(main_screen, mutex);
    }

    void render()
    {
        {
            SemaphoreGuard lock(mutex_);
            lv_scr_load(main_screen);
        }
    }

    EntityStateUpdate update(AppState state);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);

    void setWiFiNotifier(WiFiNotifier *wifi_notifier);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void triggerMotorConfigUpdate();
    void handleNavigationEvent(NavigationEvent event);

private:
    SemaphoreHandle_t mutex_;

    uint8_t current_position = 0;
    char firmware_version[16];

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config;

    WiFiNotifier *wifi_notifier;
    OSConfigNotifier *os_config_notifier;
    MotorNotifier *motor_notifier;

    void indicatorDots();
    void screen_load_task(void *param);

    lv_obj_t *overlay;
    lv_obj_t *main_screen = lv_obj_create(NULL);

    OnboardingPageManager *page_mgr = nullptr;
}