#include "./display/page_manager.h"
#include "./util.h"
#include "./notify/os_config_notifier/os_config_notifier.h"

class DemoSettingsPage : public BasePage
{
public:
    DemoSettingsPage(lv_obj_t *parent);

    void updateFromSystem(AppState state) override;
    void handleNavigation(NavigationEvent event) override;
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);

private:
    lv_obj_t *prompt_label;

    OSConfigNotifier *os_config_notifier_;
};