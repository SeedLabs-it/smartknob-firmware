#pragma once
#include "../navigation/navigation.h"
#include "app_config.h"
#include "display/page_manager.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/os_config_notifier/os_config_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"
#include "semaphore_guard.h"

enum HassOnboardingPages {
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
    ConnectQRCodePage(lv_obj_t *parent) : BasePage(parent) {
        lv_obj_t *qr = lv_qrcode_create(page, 80, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_COLOR_MAKE(0x00, 0x00, 0x00));
        char data[128];
        sprintf(data, "WIFI:T:WPA;S:%s;P:%s;H:;;", "Fam Wall", "TEST_PASSWORD");

        lv_qrcode_update(qr, data, strlen(data));
        lv_obj_center(qr);

        lv_obj_t *scan_to_connect_label = lv_label_create(page);
        lv_label_set_text(scan_to_connect_label, "SCAN TO CONNECT\nTO THE SMARTKNOB");
        lv_obj_set_style_text_color(scan_to_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(scan_to_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(scan_to_connect_label, qr, LV_ALIGN_OUT_TOP_MID, 0, -12);

        lv_obj_t *or_connect_label = lv_label_create(page);
        lv_label_set_text(or_connect_label, "OR CONNECT TO\nSSID\n12345678");
        lv_obj_set_style_text_color(or_connect_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_PART_MAIN);
        lv_obj_set_style_text_align(or_connect_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align_to(or_connect_label, qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    }
};

class WebServerQRCodePage : public BasePage
{
  public:
    WebServerQRCodePage(lv_obj_t *parent) : BasePage(parent) {
        lv_obj_t *qr = lv_qrcode_create(page, 80, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_COLOR_MAKE(0x00, 0x00, 0x00));

        char data[128];
        sprintf(data, "https://lvgl.io");
        lv_qrcode_update(qr, data, strlen(data));
        lv_obj_center(qr);
    }
};

class ContinueInBrowserWifiPage : public BasePage
{
  public:
    ContinueInBrowserWifiPage(lv_obj_t *parent) : BasePage(parent) {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "PLEASE CONTINUE WIFI\nSETUP IN BROWSER");
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};

class ConnectingToWifiPage : public BasePage
{
  public:
    ConnectingToWifiPage(lv_obj_t *parent) : BasePage(parent) {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "CONNECTING TO WIFI...");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};

class ContinueInBrowserMqttPage : public BasePage
{
  public:
    ContinueInBrowserMqttPage(lv_obj_t *parent) : BasePage(parent) {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "PLEASE CONTINUE MQTT\nSETUP IN BROWSER");
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};

class ConnectingToMqttPage : public BasePage
{
  public:
    ConnectingToMqttPage(lv_obj_t *parent) : BasePage(parent) {
        lv_obj_t *label = lv_label_create(page);
        lv_label_set_text(label, "CONNECTING TO MQTT...");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, LV_PART_MAIN);
    }
};

class HassOnboardingPageManager : public PageManager<HassOnboardingPages>
{
  public:
    HassOnboardingPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex)
        : PageManager<HassOnboardingPages>(parent, mutex) {
        add(CONNECT_QRCODE_PAGE, new ConnectQRCodePage(parent));
        add(WEBSERVER_QRCODE_PAGE, new WebServerQRCodePage(parent));
        add(CONTINUE_IN_BROWSER_WIFI_PAGE, new ContinueInBrowserWifiPage(parent));
        add(CONNECTING_TO_WIFI_PAGE, new ConnectingToWifiPage(parent));
        add(CONTINUE_IN_BROWSER_MQTT_PAGE, new ContinueInBrowserMqttPage(parent));
        add(CONNECTING_TO_MQTT_PAGE, new ConnectingToMqttPage(parent));

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