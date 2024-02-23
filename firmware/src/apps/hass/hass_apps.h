#include "../apps.h"
#include "display_buffer.h"

class HassApps : public Apps
{
public:
    HassApps(){};
#ifndef USE_DISPLAY_BUFFER
    HassApps(TFT_eSprite *spr) : Apps(spr){};
#endif
    void sync(cJSON *apps_);
};