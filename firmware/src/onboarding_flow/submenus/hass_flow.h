#pragma once
#include "../navigation/navigation.h"
#include "app_config.h"
#include "display/page_manager.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/os_config_notifier/os_config_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"
#include "semaphore_guard.h"

enum HassOnboardingPages
{
    CONNECT_QRCODE_PAGE = 0,
    WEBSERVER_QRCODE_PAGE,
    CONTINUE_IN_BROWSER_WIFI_PAGE,
    CONNECTING_TO_WIFI_PAGE,
    CONTINUE_IN_BROWSER_MQTT_PAGE,
    CONNECTING_TO_MQTT_PAGE,
    HASS_ONBOARDING_PAGE_COUNT,
    LOADING_PAGE
};

class LoadingPage : public BasePage
{
public:
    LoadingPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "LOADING...");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};

class ConnectQRCodePage : public BasePage
{
public:
    ConnectQRCodePage(lv_obj_t *parent) : BasePage(parent)
    {
        qr = lv_qrcode_create(page, 80, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));

        lv_qrcode_update(qr, "", strlen(""));
        lv_obj_center(qr);

        lv_obj_t *scan_to_connect_label = lv_label_create(page);
        lv_label_set_text(scan_to_connect_label, "SCAN TO CONNECT\nTO THE SMARTKNOB");
        lv_obj_set_style_text_color(scan_to_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(scan_to_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(scan_to_connect_label, qr, LV_ALIGN_OUT_TOP_MID, 0, -12);

        lv_obj_t *or_connect_label = lv_label_create(page);
        lv_label_set_text(or_connect_label, "OR CONNECT TO");
        lv_obj_set_style_text_color(or_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(or_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(or_connect_label, qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);

        ssid_label = lv_label_create(page);
        lv_label_set_recolor(ssid_label, true);
        lv_label_set_text(ssid_label, "SSID: SKDK_000000"); // TODO PLACEHOLDER FOR ALIGNMENT / Improve
        lv_obj_set_style_text_color(ssid_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_align_to(ssid_label, or_connect_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

        lv_obj_t *pswd_label = lv_label_create(page);
        lv_label_set_recolor(pswd_label, true);
        lv_label_set_text(pswd_label, "PSWD: #FFFFFF 12345678#");
        lv_obj_set_style_text_color(pswd_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_align_to(pswd_label, ssid_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 1);
    }

    lv_obj_t *qr;
    lv_obj_t *ssid_label;
};

class WebServerQRCodePage : public BasePage
{
public:
    WebServerQRCodePage(lv_obj_t *parent) : BasePage(parent)
    {
        qr = lv_qrcode_create(page, 80, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));

        lv_qrcode_update(qr, "", strlen(""));
        lv_obj_center(qr);

        char buf[128];

        lv_obj_t *scan_to_connect_label = lv_label_create(page);
        lv_label_set_text(scan_to_connect_label, "SCAN TO START\nSETUP");
        lv_obj_set_style_text_color(scan_to_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(scan_to_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(scan_to_connect_label, qr, LV_ALIGN_OUT_TOP_MID, 0, -12);

        lv_obj_t *or_connect_label = lv_label_create(page);
        lv_label_set_recolor(or_connect_label, true);
        sprintf(buf, "OR OPEN #FFFFFF http://192.168.4.1#\n IN YOUR BROWSER");
        lv_label_set_text(or_connect_label, buf);
        lv_obj_set_style_text_color(or_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(or_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(or_connect_label, qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    }

    lv_obj_t *qr;
};

class ContinueInBrowserWifiPage : public BasePage
{
public:
    ContinueInBrowserWifiPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_obj_set_style_text_color(label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_label_set_text(label, "CONTINUE IN\nBROWSER");
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);

        lv_obj_t *wifi_label = lv_label_create(page);
        lv_label_set_text(wifi_label, "WiFi");
        lv_obj_align(wifi_label, LV_ALIGN_BOTTOM_MID, 0, -12);
    }
};

class ConnectingToWifiPage : public BasePage
{
public:
    ConnectingToWifiPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "CONNECTING TO WIFI...");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};

class ContinueInBrowserMqttPage : public BasePage
{
public:
    ContinueInBrowserMqttPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_obj_set_style_text_color(label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_label_set_text(label, "CONTINUE IN\nBROWSER");
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);

        lv_obj_t *wifi_label = lv_label_create(page);
        lv_label_set_text(wifi_label, "MQTT");
        lv_obj_align(wifi_label, LV_ALIGN_BOTTOM_MID, 0, -12);
    }
};

class ConnectingToMqttPage : public BasePage
{
public:
    ConnectingToMqttPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "CONNECTING TO MQTT...");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};

class HassOnboardingPageManager : public PageManager<HassOnboardingPages>
{
public:
    HassOnboardingPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex)
        : PageManager<HassOnboardingPages>(parent, mutex)
    {
        add(LOADING_PAGE, new LoadingPage(parent));
        add(CONNECT_QRCODE_PAGE, new ConnectQRCodePage(parent));
        add(WEBSERVER_QRCODE_PAGE, new WebServerQRCodePage(parent));
        add(CONTINUE_IN_BROWSER_WIFI_PAGE, new ContinueInBrowserWifiPage(parent));
        add(CONNECTING_TO_WIFI_PAGE, new ConnectingToWifiPage(parent));
        add(CONTINUE_IN_BROWSER_MQTT_PAGE, new ContinueInBrowserMqttPage(parent));
        add(CONNECTING_TO_MQTT_PAGE, new ConnectingToMqttPage(parent));

        show(LOADING_PAGE);
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

    void handleEvent(WiFiEvent event);

    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void triggerMotorConfigUpdate();

    void handleNavigationEvent(NavigationEvent event);

private:
    SemaphoreHandle_t mutex_;

    PB_SmartKnobConfig root_level_motor_config;

    OSConfigNotifier *os_config_notifier;
    MotorNotifier *motor_notifier;

    lv_obj_t *overlay;
    lv_obj_t *main_screen = lv_obj_create(NULL);

    HassOnboardingPageManager *page_mgr = nullptr;
    RenderParentCallback render_parent_; // Weird workaround.

    uint8_t sta_connecting_tick = 0;
    char ap_data[64];
    char ip_data[64];
    char mqtt_server[32];
    uint8_t mqtt_connecting_tick = 0;
};