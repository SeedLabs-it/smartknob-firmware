#include "demo_apps.h"

DemoApps::DemoApps(TFT_eSprite *spr, TFT_eSPI *tft_) : HassApps(spr, tft_)
{
    HassApps::sync(cJSON_Parse(R"([{"app_slug": "climate","app_id": "climate.office","friendly_name": "Climate","area": "office","menu_color": "#ffffff","entity_id": "climate_entity_id"},{"app_slug": "stopwatch","app_id": "stopwatch.office","friendly_name": "Stopwatch","area": "office","menu_color": "#ffffff","entity_id": "stopwatch_entity_id"},{"app_slug": "light_switch","app_id": "light.ceiling","friendly_name": "Ceiling","area": "Kitchen","menu_color": "#ffffff","entity_id": "ceiling_light_entity_id"},{"app_slug": "light_dimmer","app_id": "light.workbench","friendly_name": "Workbench","area": "Kitchen","menu_color": "#ffffff","entity_id": "workbench_light_entity_id"},{"app_slug": "thermostat","app_id": "climate.office","friendly_name": "Climate","area": "Office","menu_color": "#ffffff","entity_id": "climate_entity_id"},{"app_slug": "3d_printer","app_id": "3d_printer.office","friendly_name": "3D Printer","area": "Office","menu_color": "#ffffff","entity_id": "printer_entity_id"},{"app_slug": "blinds","app_id": "blinds.office","friendly_name": "Shades","area": "Office","menu_color": "#ffffff","entity_id": "blinds_entity_id"},{"app_slug": "music","app_id": "music.office","friendly_name": "Music","area": "Office","menu_color": "#ffffff","entity_id": "music_entity_id"}])"));
}

void DemoApps::handleNavigationEvent(NavigationEvent event)
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

void DemoApps::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
}