#include "../apps.h"

class HassApps : public Apps
{
public:
    HassApps(){};
    HassApps(TFT_eSprite *spr) : Apps(spr){};
    void sync(cJSON *apps_);
};