#pragma once
#include "apps.h"
#include "../util.h"
#include "../events/events.h"

class HassApps : public Apps
{
public:
    HassApps(SemaphoreHandle_t mutex);

    void sync(cJSON *json_apps);
    void handleEvent(WiFiEvent event);
    void handleNavigationEvent(NavigationEvent event);
    void render();

private:
    lv_obj_t *waiting_for_hass = lv_obj_create(NULL);

    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);
};