#pragma once
#include "../hass/hass_apps.h"
#include "notify/os_config_notifier/os_config_notifier.h"

class DemoApps : public HassApps
{
public:
    // DemoApps(){};
    DemoApps(SemaphoreHandle_t mutex) : HassApps(mutex)
    {
        HassApps::sync(cJSON_Parse(R"([{"app_slug": "light_switch","app_id": "light.ceiling","friendly_name": "Ceiling","area": "Kitchen","menu_color": "#ffffff","entity_id": "ceiling_light_entity_id"}])"));
    }
    void handleNavigationEvent(NavigationEvent event)
    {
        if (event.press == NAVIGATION_EVENT_PRESS_LONG)
        {
            if (active_id == MENU)
            {
                os_config_notifier->setOSMode(Onboarding);
                return;
            }
        }
        Apps::handleNavigationEvent(event);
    }
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
    {
        this->os_config_notifier = os_config_notifier;
    }

private:
    OSConfigNotifier *os_config_notifier;
};