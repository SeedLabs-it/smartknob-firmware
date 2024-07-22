#include "./display/page_manager.h"
#include "./util.h"

class DemoSettingsPage : public BasePage
{
public:
    DemoSettingsPage(lv_obj_t *parent);

    void updateFromSystem(AppState state) override;
    void handleNavigation(NavigationEvent event) override;

    BasePage *in_demo_mode_page;
};

class InDemoModeSettingsPage : public BasePage
{
public:
    InDemoModeSettingsPage(lv_obj_t *parent, BasePage *demo_page);

    void updateFromSystem(AppState state) override;

private:
    BasePage *demo_page_;
    lv_obj_t *demo_mode_label;
};