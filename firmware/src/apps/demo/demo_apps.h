#pragma once
#include "../hass/hass_apps.h"
#include "notify/os_config_notifier/os_config_notifier.h"

class DemoApps : public HassApps
{
public:
    DemoApps(){};
    DemoApps(TFT_eSprite *spr);
    void handleNavigationEvent(NavigationEvent event);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);

private:
    OSConfigNotifier *os_config_notifier;
};