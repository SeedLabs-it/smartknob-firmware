#include "../apps.h"

class HassApps : public Apps
{
public:
    HassApps();
    void sync(cJSON *apps_);
};