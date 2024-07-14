#pragma once
#include "hass_apps.h"
#include "notify/os_config_notifier/os_config_notifier.h"

class DemoApps : public HassApps
{
public:
    // DemoApps(){};
    DemoApps(SemaphoreHandle_t mutex);
    void handleNavigationEvent(NavigationEvent event);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);

private:
    OSConfigNotifier *os_config_notifier;
};