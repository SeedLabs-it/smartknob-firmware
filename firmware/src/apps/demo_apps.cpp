#include "demo_apps.h"
#include "demo_motor_config_app.h"

DemoApps::DemoApps(SemaphoreHandle_t mutex) : HassApps(mutex)
{
    auto *motor_app = new DemoMotorConfigApp(mutex);
    motor_app->setBack(DONT_NAVIGATE);
    motor_app->setNext(DONT_NAVIGATE);
    add(0, motor_app);
    setActive(0);
}

void DemoApps::render()
{
    if (active_app == nullptr)
    {
        return;
    }
    Apps::render();
}

void DemoApps::handleNavigationEvent(NavigationEvent event)
{
    switch (event)
    {
    case NavigationEvent::LONG:
        if (active_id == MENU)
        {
            os_config_notifier->setOSMode(ONBOARDING);
            return;
        }
        break;
    }
    Apps::handleNavigationEvent(event);
}

void DemoApps::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}
