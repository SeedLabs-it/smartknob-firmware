#pragma once
#include "../apps.h"
#include "../../events/events.h"

class HassApps : public Apps
{
public:
    HassApps(){};
    HassApps(TFT_eSprite *spr) : Apps(spr){};
    void sync(cJSON *apps_);
    void handleEvent(WiFiEvent event);
};