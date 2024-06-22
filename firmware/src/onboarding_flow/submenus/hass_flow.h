#pragma once
#include "display/page_manager.h"
#include "app_config.h"
#include "../navigation/navigation.h"
#include "semaphore_guard.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"
#include "notify/os_config_notifier/os_config_notifier.h"

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

class ConnectQRCodePage : public BasePage
{
public:
    ConnectQRCodePage(lv_obj_t *parent) : BasePage(parent)
    {
        // lv_obj_t *qr = lv_qrcode_create(page);
        // lv_qrcode_set_size(qr, 80);
        // lv_qrcode_set_dark_color(qr, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));
        // lv_qrcode_set_light_color(qr, LV_COLOR_MAKE(0x00, 0x00, 0x00));

        // char data[128];
        // sprintf(data, "WIFI:T:WPA;S:%s;P:%s;H:;;", "Fam Wall", "TEST_PASSWORD");

        // lv_qrcode_update(qr, data, strlen(data));
        // lv_obj_center(qr);

        lv_obj_t *scan_to_connect_label = lv_label_create(page);
        lv_label_set_text(scan_to_connect_label, "SCAN TO CONNECT\nTO THE SMARTKNOB");
        lv_obj_set_style_text_align(scan_to_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        // lv_obj_align_to(scan_to_connect_label, qr, LV_ALIGN_OUT_TOP_MID, 0, -12);

        lv_obj_t *or_connect_label = lv_label_create(page);
        lv_label_set_text(or_connect_label, "OR CONNECT TO\nSSID\n12345678");
        lv_obj_set_style_text_align(or_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        // lv_obj_align_to(or_connect_label, qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    }
};

class WebServerQRCodePage : public BasePage
{
public:
    WebServerQRCodePage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
        lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

        // lv_obj_t *qr = lv_qrcode_create(page);
        // lv_qrcode_set_size(qr, 150);
        // lv_qrcode_set_dark_color(qr, fg_color);
        // lv_qrcode_set_light_color(qr, bg_color);

        // /*Set data*/
        // const char *data = "https://lvgl.io";
        // lv_qrcode_update(qr, data, strlen(data));
        // lv_obj_center(qr);

        // /*Add a border with bg_color*/
        // lv_obj_set_style_border_color(qr, bg_color, 0);
        // lv_obj_set_style_border_width(qr, 5, 0);
    }
};

class HassOnboardingPageManager : public PageManager<HassOnboardingPages>
{
public:
    HassOnboardingPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex) : PageManager<HassOnboardingPages>(parent, mutex)
    {
        add(CONNECT_QRCODE_PAGE, new ConnectQRCodePage(parent));
        add(WEBSERVER_QRCODE_PAGE, new WebServerQRCodePage(parent));

        show(CONNECT_QRCODE_PAGE);
    }
};

typedef std::function<void(void)> RenderParentCallback;

class HassOnboardingFlow
{
public:
    HassOnboardingFlow(SemaphoreHandle_t mutex, RenderParentCallback render_parent);

    void render();

    EntityStateUpdate update(AppState state);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);

    // void setWiFiNotifier(WiFiNotifier *wifi_notifier);
    // void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void triggerMotorConfigUpdate();

    void handleNavigationEvent(NavigationEvent event);

private:
    SemaphoreHandle_t mutex_;

    uint8_t current_position = 0;

    PB_SmartKnobConfig root_level_motor_config;

    // WiFiNotifier *wifi_notifier;
    // OSConfigNotifier *os_config_notifier;
    MotorNotifier *motor_notifier;

    lv_obj_t *overlay;
    lv_obj_t *main_screen = lv_obj_create(NULL);

    HassOnboardingPageManager *page_mgr = nullptr;
    RenderParentCallback render_parent_; // Weird workaround.
};