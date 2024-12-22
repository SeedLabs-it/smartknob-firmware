#include "wifi_flow.h"

WiFiOnboardingPages getWiFiPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= WIFI_ONBOARDING_PAGES_COUNT - 1)
    {
        return static_cast<WiFiOnboardingPages>(screen);
    }
    return CONNECT_QRCODE_PAGE; // TODO handle error here instead of returning CONNECT_QRCODE_PAGE
};

WiFiOnboardingFlow::WiFiOnboardingFlow(SemaphoreHandle_t mutex, RenderParentCallback render_parent)
    : mutex_(mutex), render_parent_(render_parent)
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

    page_mgr = new WiFiOnboardingPageManager(main_screen, mutex);
}

void WiFiOnboardingFlow::render()
{

    SemaphoreGuard lock(mutex_);
    lv_scr_load(main_screen);
    triggerMotorConfigUpdate();
}

void WiFiOnboardingFlow::handleEvent(WiFiEvent event)
{
    ConnectQRCodePage *page_connect = (ConnectQRCodePage *)page_mgr->getPage(CONNECT_QRCODE_PAGE);
    WebServerQRCodePage *page_server = (WebServerQRCodePage *)page_mgr->getPage(WEBSERVER_QRCODE_PAGE);

    switch (event.type)
    {
    case SK_WIFI_AP_STARTED:
        sprintf(ap_data, "WIFI:T:WPA;S:%s;P:%s;H:;;", event.body.wifi_ap_started.ssid, event.body.wifi_ap_started.passphrase);
        lv_qrcode_update(page_connect->qr, ap_data, strlen(ap_data));

        lv_label_set_text_fmt(page_connect->ssid_label, "SSID: #FFFFFF %s#", event.body.wifi_ap_started.ssid);

        page_mgr->show(getWiFiPageEnum(CONNECT_QRCODE_PAGE));

        break;
    case SK_AP_CLIENT:
        if (event.body.ap_client.connected)
        {
            sprintf(ip_data, "%s", "http://192.168.4.1/?wifi", WiFi.localIP().toString().c_str()); // always the same
            lv_qrcode_update(page_server->qr, ip_data, strlen(ip_data));

            page_mgr->show(getWiFiPageEnum(WEBSERVER_QRCODE_PAGE));
        }
        else
        {
            page_mgr->show(getWiFiPageEnum(CONNECT_QRCODE_PAGE));
        }
        break;
    case SK_WEB_CLIENT:
        if (event.body.ap_client.connected)
        {
            page_mgr->show(getWiFiPageEnum(WEBSERVER_QRCODE_PAGE));
        }
        else
        {
            page_mgr->show(getWiFiPageEnum(CONNECT_QRCODE_PAGE));
        }
        break;
    case SK_WIFI_STA_TRY_NEW_CREDENTIALS:                                 // Credentials for connect event available in body. If we want to display for user.
        sta_connecting_tick = event.body.wifi_sta_connecting.retry_count; // Will leave if i or someone wants to add countdown timer for connecting event.

        page_mgr->show(getWiFiPageEnum(CONNECTING_TO_WIFI_PAGE));
        break;
    case SK_WIFI_STA_CONNECTED_NEW_CREDENTIALS:
        cb_();
        break;
    default:
        break;
    }
}

void WiFiOnboardingFlow::handleNavigationEvent(NavigationEvent event)
{
    switch (event)
    {
    case NavigationEvent::LONG:
        render_parent_();
        break;
    }
}

EntityStateUpdate WiFiOnboardingFlow::update(AppState state)
{
    return updateStateFromKnob(state.motor_state);
}

EntityStateUpdate WiFiOnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    if (page_mgr->getCurrentPageNum() != state.current_position)
    {
        page_mgr->show(getWiFiPageEnum(state.current_position));
    }

    EntityStateUpdate new_state;
    return new_state;
}

void WiFiOnboardingFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
}

void WiFiOnboardingFlow::triggerMotorConfigUpdate()
{
    if (this->motor_notifier != nullptr)
    {
        motor_notifier->requestUpdate(root_level_motor_config);
    }
    else
    {
        LOGW("Motor_notifier is not set");
    }
}

void WiFiOnboardingFlow::setOSConfigNotifier(OSConfigNotifier
                                                 *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}

void WiFiOnboardingFlow::setCallback(Callback cb)
{
    cb_ = cb;
}