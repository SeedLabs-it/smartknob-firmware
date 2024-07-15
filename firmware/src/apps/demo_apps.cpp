#include "demo_apps.h"

DemoApps::DemoApps(SemaphoreHandle_t mutex) : HassApps(mutex)
{
    HassApps::sync(cJSON_Parse(R"([{"app_slug": "stopwatch","app_id": "light.ceiling1","friendly_name": "Ceiling1","area": "Kitchen1","menu_color": "#ffffff","entity_id": "stopwatch"},{"app_slug": "light_switch","app_id": "light.ceiling","friendly_name": "Ceiling","area": "Kitchen","menu_color": "#ffffff","entity_id": "ceiling_light_entity_id"},{"app_slug": "light_dimmer","app_id": "light.workbench","friendly_name": "Workbench","area": "Kitchen","menu_color": "#ffffff","entity_id": "workbench_light_entity_id"}])"));
}

void DemoApps::handleNavigationEvent(NavigationEvent event)
{
    if (event.press == NAVIGATION_EVENT_PRESS_LONG)
    {
        if (active_id == MENU)
        {
            os_config_notifier->setOSMode(ONBOARDING);
            return;
        }
    }
    Apps::handleNavigationEvent(event);
}

void DemoApps::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}