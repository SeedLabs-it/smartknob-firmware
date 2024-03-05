#include "../apps.h"
#include "../../events/events.h"
#include "notify/os_config_notifier/os_config_notifier.h"

class HassApps : public Apps
{
public:
    HassApps(){};
    HassApps(TFT_eSprite *spr) : Apps(spr){};
    void sync(cJSON *apps_);
    void handleEvent(WiFiEvent event);
    void handleNavigationEvent(NavigationEvent event);

    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);

private:
    OSConfigNotifier *os_config_notifier;
};