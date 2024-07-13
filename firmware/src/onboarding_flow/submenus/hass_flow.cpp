#include "hass_flow.h"

HassOnboardingPages getHassPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= HASS_ONBOARDING_PAGE_COUNT - 1)
    {
        return static_cast<HassOnboardingPages>(screen);
    }
};

HassOnboardingFlow::HassOnboardingFlow(SemaphoreHandle_t mutex, RenderParentCallback render_parent)
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

    page_mgr = new HassOnboardingPageManager(main_screen, mutex);
}

void HassOnboardingFlow::render()
{

    SemaphoreGuard lock(mutex_);
    lv_scr_load(main_screen);
    triggerMotorConfigUpdate();
}

void HassOnboardingFlow::handleEvent(WiFiEvent event)
{
    ConnectQRCodePage *page_connect = (ConnectQRCodePage *)page_mgr->getPage(CONNECT_QRCODE_PAGE);
    WebServerQRCodePage *page_server = (WebServerQRCodePage *)page_mgr->getPage(WEBSERVER_QRCODE_PAGE);

    switch (event.type)
    {
    case SK_WIFI_AP_STARTED:
        LOGE("SK_WIFI_AP_STARTED");
        is_wifi_ap_started = true;
        sprintf(wifi_ap_ssid, "%s", event.body.wifi_ap_started.ssid);
        sprintf(wifi_ap_passphrase, "%s", event.body.wifi_ap_started.passphrase);
        sprintf(ap_data, "WIFI:T:WPA;S:%s;P:%s;H:;;", wifi_ap_ssid, wifi_ap_passphrase);
        // setQRCode(ap_data);
        lv_qrcode_update(page_connect->qr, ap_data, strlen(ap_data));

        page_mgr->show(getHassPageEnum(CONNECT_QRCODE_PAGE));
        // // std::string wifiqrcode_test = "WIFI:T:WPA;S:SMARTKNOB-AP;P:smartknob;H:;;";

        break;
    case SK_AP_CLIENT:
        is_wifi_ap_client_connected = event.body.ap_client.connected;
        if (is_wifi_ap_client_connected)
        {
            if (is_wifi_ap_client_connected)
            {
                sprintf(ip_data, "%s", "http://192.168.4.1"); // always the same
            }
            else
            {
                sprintf(ip_data, "http://%s/", WiFi.localIP().toString().c_str());
            }

            lv_qrcode_update(page_server->qr, ip_data, strlen(ip_data));

            page_mgr->show(getHassPageEnum(WEBSERVER_QRCODE_PAGE));
        }
        else
        {
            page_mgr->show(getHassPageEnum(CONNECT_QRCODE_PAGE));
        }
        break;
    case SK_WEB_CLIENT:
        is_web_client_connected = event.body.ap_client.connected;
        if (is_web_client_connected)
        {
            page_mgr->show(getHassPageEnum(WEBSERVER_QRCODE_PAGE));
        }
        else
        {
            page_mgr->show(getHassPageEnum(CONNECT_QRCODE_PAGE));
        }
        break;
    case SK_WIFI_STA_TRY_NEW_CREDENTIALS:
        sta_connecting_tick = event.body.wifi_sta_connecting.retry_count;
        sprintf(wifi_sta_ssid, "%s", event.body.wifi_sta_connecting.ssid);
        sprintf(wifi_sta_passphrase, "%s", event.body.wifi_sta_connecting.passphrase);

        page_mgr->show(getHassPageEnum(CONNECTING_TO_WIFI_PAGE));
        break;
    case SK_WIFI_STA_TRY_NEW_CREDENTIALS_FAILED:
        // new_wifi_credentials_failed = true;
        // current_page = ONBOARDING_FLOW_PAGE_STEP_HASS_4;
        break;
    case SK_WEB_CLIENT_MQTT:
        page_mgr->show(getHassPageEnum(CONTINUE_IN_BROWSER_MQTT_PAGE));
        break;
    case SK_MQTT_TRY_NEW_CREDENTIALS:
        sprintf(mqtt_server, "%s:%d", event.body.mqtt_connecting.host, event.body.mqtt_connecting.port);
        page_mgr->show(getHassPageEnum(CONNECTING_TO_MQTT_PAGE));
        break;
    case SK_MQTT_TRY_NEW_CREDENTIALS_FAILED:
        // new_mqtt_credentials_failed = true;
        // current_page = ONBOARDING_FLOW_PAGE_STEP_HASS_6;
        break;
    case SK_MQTT_CONNECTED_NEW_CREDENTIALS:
        // current_page = ONBOARDING_FLOW_PAGE_STEP_HASS_8;
        sprintf(mqtt_server, "%s:%d", event.body.mqtt_connecting.host, event.body.mqtt_connecting.port);
        // is_onboarding_finished = true;
        os_config_notifier->setOSMode(HASS);

        break;
    case SK_MQTT_STATE_UPDATE:
        break;
    default:
        break;
    }
}

void HassOnboardingFlow::handleNavigationEvent(NavigationEvent event)
{
    if (event.press == NAVIGATION_EVENT_PRESS_LONG)
    {
        if (page_mgr->getCurrentPageNum() == 0)
        {
            render_parent_();
            return;
        }
    }

    if (event.press == NAVIGATION_EVENT_PRESS_SHORT) // ! REMOVE JUST FOR TESTING
    {
        uint8_t page_num = page_mgr->getCurrentPageNum();
        if (page_num == 0)
        {
            page_mgr->show(getHassPageEnum(WEBSERVER_QRCODE_PAGE));
        }
        else if (page_num == WEBSERVER_QRCODE_PAGE)
        {
            page_mgr->show(getHassPageEnum(CONTINUE_IN_BROWSER_WIFI_PAGE));
        }
        else if (page_num == CONTINUE_IN_BROWSER_WIFI_PAGE)
        {
            page_mgr->show(getHassPageEnum(CONNECTING_TO_WIFI_PAGE));
        }
        else if (page_num == CONNECTING_TO_WIFI_PAGE)
        {
            page_mgr->show(getHassPageEnum(CONTINUE_IN_BROWSER_MQTT_PAGE));
        }
        else if (page_num == CONTINUE_IN_BROWSER_MQTT_PAGE)
        {
            page_mgr->show(getHassPageEnum(CONNECTING_TO_MQTT_PAGE));
        }
        else if (page_num == CONNECTING_TO_MQTT_PAGE)
        {
            page_mgr->show(getHassPageEnum(0));
        }
    }
}

EntityStateUpdate HassOnboardingFlow::update(AppState state)
{
    // updateStateFromSystem(state);
    return updateStateFromKnob(state.motor_state);
}

EntityStateUpdate HassOnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    if (page_mgr->getCurrentPageNum() != state.current_position)
    {
        page_mgr->show(getHassPageEnum(state.current_position));
    }

    EntityStateUpdate new_state;
    return new_state;
}

void HassOnboardingFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
}

void HassOnboardingFlow::triggerMotorConfigUpdate()
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

// void HassOnboardingFlow::setWiFiNotifier(WiFiNotifier *wifi_notifier)
// {
//     this->wifi_notifier = wifi_notifier;
// }

void HassOnboardingFlow::setOSConfigNotifier(OSConfigNotifier
                                                 *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}