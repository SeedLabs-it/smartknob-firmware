#pragma once
#include "../navigation/navigation.h"
#include "app_config.h"
#include "display/page_manager.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/os_config_notifier/os_config_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"
#include "semaphore_guard.h"

enum SpotifyOnboardingPages
{
    SPOTIFY_CREDS_WEBSERVER_PAGE = 0,
    SPOTIFY_ONBOARDING_PAGE_COUNT,
    SPOTIFY_LOADING_PAGE
};

class SpotifyLoadingPage : public BasePage
{
public:
    SpotifyLoadingPage(lv_obj_t *parent) : BasePage(parent)
    {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "LOADING...");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};
class SpotifyCredsWebServerQRCodePage : public BasePage
{
public:
    SpotifyCredsWebServerQRCodePage(lv_obj_t *parent) : BasePage(parent)
    {
        qr = lv_qrcode_create(page, 80, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));

        lv_qrcode_update(qr, "", strlen(""));
        lv_obj_center(qr);

        char buf[128];

        lv_obj_t *scan_to_connect_label = lv_label_create(page);
        lv_label_set_text(scan_to_connect_label, "BROWSER SHOULD BE\nREDIRECTING TO KNOB WIFI IP");
        lv_obj_set_style_text_color(scan_to_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(scan_to_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(scan_to_connect_label, qr, LV_ALIGN_OUT_TOP_MID, 0, -12);

        or_connect_label = lv_label_create(page);
        lv_label_set_recolor(or_connect_label, true);
        snprintf(buf, sizeof(buf), "SCAN OR\nOPEN #FFFFFF http://%s#\n IN YOUR BROWSER", WiFi.localIP().toString().c_str());
        lv_label_set_text(or_connect_label, buf);
        lv_obj_set_style_text_color(or_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(or_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(or_connect_label, qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    }

    lv_obj_t *qr;
    lv_obj_t *or_connect_label;
};

class SpotifyContinueInBrowserWifiPage : public BasePage
{
public:
    SpotifyContinueInBrowserWifiPage(lv_obj_t *parent) : BasePage(parent)
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

class SpotifyOnboardingPageManager : public PageManager<SpotifyOnboardingPages>
{
public:
    SpotifyOnboardingPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex)
        : PageManager<SpotifyOnboardingPages>(parent, mutex)
    {
        add(SPOTIFY_LOADING_PAGE, new SpotifyLoadingPage(parent));
        add(SPOTIFY_CREDS_WEBSERVER_PAGE, new SpotifyCredsWebServerQRCodePage(parent));
        // add(CONNECT_QRCODE_PAGE, new ConnectQRCodePage(parent));
        // add(WEBSERVER_QRCODE_PAGE, new WebServerQRCodePage(parent));
        // add(CONTINUE_IN_BROWSER_WIFI_PAGE, new ContinueInBrowserWifiPage(parent));
        // add(CONNECTING_TO_WIFI_PAGE, new ConnectingToWifiPage(parent));
        // add(CONTINUE_IN_BROWSER_MQTT_PAGE, new ContinueInBrowserMqttPage(parent));
        // add(CONNECTING_TO_MQTT_PAGE, new ConnectingToMqttPage(parent));

        show(SPOTIFY_CREDS_WEBSERVER_PAGE);
    }
};

typedef std::function<void(void)> RenderParentCallback;

class SpotifyOnboardingFlow
{
public:
    SpotifyOnboardingFlow(SemaphoreHandle_t mutex, RenderParentCallback render_parent);

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

    SpotifyOnboardingPageManager *page_mgr = nullptr;
    RenderParentCallback render_parent_; // Weird workaround.

    uint8_t sta_connecting_tick = 0;
    char ap_data[64];
    char ip_data[64];
    char mqtt_server[32];
    uint8_t mqtt_connecting_tick = 0;
};