#include "hass_flow.h"

HassOnboardingPages getHassPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= HASS_ONBOARDING_PAGE_COUNT - 1)
    {
        return static_cast<HassOnboardingPages>(screen);
    }
};

HassOnboardingFlow::HassOnboardingFlow(SemaphoreHandle_t mutex) : mutex_(mutex)
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

EntityStateUpdate HassOnboardingFlow::update(AppState state)
{
    // updateStateFromSystem(state);
    return updateStateFromKnob(state.motor_state);
}

EntityStateUpdate HassOnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    if (current_position != state.current_position)
    {
        current_position = state.current_position;
        page_mgr->show(getHassPageEnum(current_position));
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

// void HassOnboardingFlow::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
// {
//     this->os_config_notifier = os_config_notifier;
// }