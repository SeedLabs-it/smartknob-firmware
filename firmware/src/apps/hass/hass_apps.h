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

    TFT_eSprite *renderActive();

private:
    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);

    TFT_eSprite *renderWaitingForHass();
};