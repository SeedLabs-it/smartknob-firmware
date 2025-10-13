#include "hass_flow.h"

HassOnboardingPages getHassPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= HASS_ONBOARDING_PAGE_COUNT - 1)
    {
        return static_cast<HassOnboardingPages>(screen);
    }
    // return CONNECT_QRCODE_PAGE; // TODO handle error here instead of returning CONNECT_QRCODE_PAGE
    return CONTINUE_IN_BROWSER_MQTT_PAGE; // TODO handle error here instead of returning CONNECT_QRCODE_PAGE
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
    switch (event.type)
    {
    case SK_WEB_CLIENT_MQTT:
        page_mgr->show(getHassPageEnum(CONTINUE_IN_BROWSER_MQTT_PAGE));
        break;
    case SK_MQTT_TRY_NEW_CREDENTIALS:
        sprintf(mqtt_server, "%s:%d", event.body.mqtt_connecting.host, event.body.mqtt_connecting.port);
        page_mgr->show(getHassPageEnum(CONNECTING_TO_MQTT_PAGE));
        break;
    case SK_MQTT_CONNECTED_NEW_CREDENTIALS:
        sprintf(mqtt_server, "%s:%d", event.body.mqtt_connecting.host, event.body.mqtt_connecting.port);
        os_config_notifier->setOSMode(HASS);
        break;
    default:
        break;
    }
}

void HassOnboardingFlow::handleNavigationEvent(NavigationEvent event)
{
    switch (event)
    {
    case NavigationEvent::LONG:
        render_parent_();
        break;
    }
}

EntityStateUpdate HassOnboardingFlow::update(AppState state)
{
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

void HassOnboardingFlow::setOSConfigNotifier(OSConfigNotifier
                                                 *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}