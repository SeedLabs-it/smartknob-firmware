#include "demo_apps.h"

void DemoApps::handleNavigationEvent(NavigationEvent event)
{
    if (event.press == NAVIGATION_EVENT_PRESS_LONG)
    {
        ESP_LOGD("demo_apps.cpp", "active_id: %d", active_id);
        if (active_id == MENU)
            os_config_notifier->setOSMode(Onboarding);
        motor_notifier->requestUpdate(active_app->getMotorConfig());
    }
    Apps::handleNavigationEvent(event);
}

void DemoApps::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}