#include "onboarding_flow.h"
#include <semaphore_guard.h>

OnboardingFlow::OnboardingFlow(SemaphoreHandle_t mutex) : mutex_(mutex)
{
    root_level_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        ONBOARDING_FLOW_PAGE_COUNT - 1,
        35 * PI / 180,
        2,
        1,
        0.55,
        "ONBOARDING",
        0,
        {},
        0,
        20,
    };

    blocked_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        55 * PI / 180,
        0.01,
        0.6,
        1.1,
        "ONBOARDING",
        0,
        {},
        0,
        90,
    };

    page_mgr = new OnboardingPageManager(main_screen, mutex);

    hass_flow = new HassOnboardingFlow(mutex, [this]()
                                       { this->render(); this->triggerMotorConfigUpdate(); });
    // spotify_flow = new SpotifyOnboardingFlow(mutex, [this]()
    //                                          { this->render(); this->triggerMotorConfigUpdate(); });

    wifi_flow = new WiFiOnboardingFlow(mutex, [this]()
                                       { this->render(); this->triggerMotorConfigUpdate(); }, [this]()
                                       { hass_flow->render(); });
#ifdef RELEASE_VERSION
    sprintf(firmware_version, "%s", RELEASE_VERSION);
#else
    sprintf(firmware_version, "%s", "DEV");
#endif
}

OnboardingFlowPages getPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= ONBOARDING_FLOW_PAGE_COUNT - 1)
    {
        return static_cast<OnboardingFlowPages>(screen);
    }
    return WELCOME_PAGE; // TODO handle error here instead of returning WELCOME_PAGE
}

void OnboardingFlow::render()
{
    root_level_motor_config.position = current_position;
    motor_notifier->requestUpdate(root_level_motor_config); // Prevents state after moving back from submenus to be reset to page 0, i.e. moves user to correct page on the onboarding menu.

    active_sub_menu = NONE;
    page_mgr->show(getPageEnum(current_position));

    {
        SemaphoreGuard lock(mutex_);
        lv_scr_load(main_screen);
    }
}

void OnboardingFlow::handleNavigationEvent(NavigationEvent event)
{
    if (active_sub_menu == HASS_SUB_MENU || active_sub_menu == SPOTIFY_SUB_MENU)
    {
        hass_flow->handleNavigationEvent(event);
        return;
    }
    if (active_sub_menu == NONE)
    {
        switch (event)
        {
        case NavigationEvent::SHORT:
            switch (getPageEnum(current_position))
            {
            case WELCOME_PAGE: // No submenus available for welcome page nor about page.
            case ABOUT_PAGE:
                break;
            case HASS_PAGE:
                active_sub_menu = HASS_SUB_MENU;
                // hass_flow->render();
                wifi_flow->setCallback([this]()
                                       { hass_flow->render(); });
                wifi_flow->render();

                wifi_notifier->requestAP();
                break;
            case SPOTIFY_PAGE:
                active_sub_menu = SPOTIFY_SUB_MENU;
                // spotify_flow->render();
                wifi_flow->setCallback([this]()
                                       { LOGE("Spotify callback"); });
                wifi_flow->render();

                wifi_notifier->requestAP();
                break;
            case DEMO_PAGE:
                os_config_notifier->setOSMode(OSMode::DEMO);
                break;

                break;
            default:
                LOGE("Unhandled navigation event");
                break;
            }
        }
    }
}

EntityStateUpdate OnboardingFlow::update(AppState state)
{
    return updateStateFromKnob(state.motor_state);
}

EntityStateUpdate OnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    if (current_position != state.current_position)
    {
        current_position = state.current_position;
        page_mgr->show(getPageEnum(current_position));
    }

    EntityStateUpdate new_state;
    return new_state;
}

void OnboardingFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
    hass_flow->setMotorNotifier(motor_notifier); // TODO: BAD WAY? FIX
    wifi_flow->setMotorNotifier(motor_notifier); // TODO: BAD WAY? FIX
}

void OnboardingFlow::triggerMotorConfigUpdate()
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

void OnboardingFlow::setWiFiNotifier(WiFiNotifier *wifi_notifier)
{
    this->wifi_notifier = wifi_notifier;
}

void OnboardingFlow::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
    hass_flow->setOSConfigNotifier(os_config_notifier);
}

void OnboardingFlow::handleEvent(WiFiEvent event)
{
    switch (event.type)
    {
    case SK_WIFI_AP_STARTED:
    case SK_AP_CLIENT:
    case SK_WEB_CLIENT:
    case SK_WIFI_STA_TRY_NEW_CREDENTIALS:
    case SK_WIFI_STA_TRY_NEW_CREDENTIALS_FAILED:
    case SK_WIFI_STA_CONNECTED_NEW_CREDENTIALS:
    case SK_WEB_CLIENT_MQTT:
    case SK_MQTT_TRY_NEW_CREDENTIALS:
    case SK_MQTT_CONNECTED_NEW_CREDENTIALS:
    case SK_MQTT_STATE_UPDATE:
        wifi_flow->handleEvent(event);
        hass_flow->handleEvent(event);
        // spotify_flow->handleEvent(event);
        break;
    default:
        break;
    }
}