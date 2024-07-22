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
    BasePage *in_demo_mode_page;
    lv_obj_t *prompt_label;

    OSConfigNotifier *os_config_notifier_;
};

class InDemoModeSettingsPage : public BasePage // Ugly workaround
{
public:
    InDemoModeSettingsPage(lv_obj_t *parent, BasePage *demo_page, OSConfigNotifier *os_config_notifier);

    void updateFromSystem(AppState state) override;

private:
    BasePage *demo_page_;
    lv_obj_t *demo_mode_label;

    OSConfigNotifier *os_config_notifier_;
};
