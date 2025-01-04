#include "spotify_flow.h"

SpotifyOnboardingPages getSpotifyPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= SPOTIFY_ONBOARDING_PAGE_COUNT - 1)
    {
        return static_cast<SpotifyOnboardingPages>(screen);
    }
    return SPOTIFY_CREDS_WEBSERVER_PAGE; // TODO handle error here instead of returning SPOTIFY_CREDS_WEBSERVER_PAGEs
};

SpotifyOnboardingFlow::SpotifyOnboardingFlow(SemaphoreHandle_t mutex, RenderParentCallback render_parent)
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

    page_mgr = new SpotifyOnboardingPageManager(main_screen, mutex);
}

void SpotifyOnboardingFlow::render()
{

    SemaphoreGuard lock(mutex_);
    lv_scr_load(main_screen);
    triggerMotorConfigUpdate();
}

void SpotifyOnboardingFlow::handleEvent(WiFiEvent event)
{
    // ConnectQRCodePage *page_connect = (ConnectQRCodePage *)page_mgr->getPage(CONNECT_QRCODE_PAGE);
    // WebServerQRCodePage *page_server = (WebServerQRCodePage *)page_mgr->getPage(WEBSERVER_QRCODE_PAGE);
    SpotifyCredsWebServerQRCodePage *page_server = (SpotifyCredsWebServerQRCodePage *)page_mgr->getPage(SPOTIFY_CREDS_WEBSERVER_PAGE);

    switch (event.type)
    {
    case SK_WIFI_STA_CONNECTED_NEW_CREDENTIALS:

        sprintf(ip_data, "http://%s/?spotify", WiFi.localIP().toString().c_str()); // always the same
        lv_qrcode_update(page_server->qr, ip_data, strlen(ip_data));

        lv_label_set_text_fmt(page_server->or_connect_label, "SCAN OR\nOPEN #FFFFFF http://%s#\n IN YOUR BROWSER", WiFi.localIP().toString().c_str());
        lv_obj_align_to(page_server->or_connect_label, page_server->qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
        break;

    default:
        break;
    }
}

void SpotifyOnboardingFlow::handleNavigationEvent(NavigationEvent event)
{
    switch (event)
    {
    case NavigationEvent::LONG:
        render_parent_();
        break;
    }
}

EntityStateUpdate SpotifyOnboardingFlow::update(AppState state)
{
    return updateStateFromKnob(state.motor_state);
}

EntityStateUpdate SpotifyOnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    if (page_mgr->getCurrentPageNum() != state.current_position)
    {
        page_mgr->show(getSpotifyPageEnum(state.current_position));
    }

    EntityStateUpdate new_state;
    return new_state;
}

void SpotifyOnboardingFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
}

void SpotifyOnboardingFlow::triggerMotorConfigUpdate()
{
    if (this->motor_notifier != nullptr)
        motor_notifier->requestUpdate(root_level_motor_config);

    else
    {
        LOGW("Motor_notifier is not set");
    }
}

void SpotifyOnboardingFlow::setOSConfigNotifier(OSConfigNotifier
                                                    *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}