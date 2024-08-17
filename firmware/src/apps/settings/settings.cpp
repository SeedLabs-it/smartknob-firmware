#include "settings.h"

SettingsApp::SettingsApp(SemaphoreHandle_t mutex) : App(mutex)
{
    sprintf(app_id, "%s", "settings");
    sprintf(entity_id, "%s", "settings");
    sprintf(friendly_name, "%s", "Settings");

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        SETTINGS_PAGE_COUNT - 1,
        35 * PI / 180,
        2,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
        "",
        0,
        {},
        0,
        20,
    };
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    LV_IMG_DECLARE(x80_settings);
    LV_IMG_DECLARE(x40_settings);

    big_icon = x80_settings;
    small_icon = x40_settings;
}

SettingsPages getSettingsPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= SETTINGS_PAGE_COUNT - 1)
    {
        return static_cast<SettingsPages>(screen);
    }
}

EntityStateUpdate SettingsApp::updateStateFromKnob(PB_SmartKnobState state)
{
    if (current_position != state.current_position)
    {
        current_position = state.current_position;
        page_mgr->show(getSettingsPageEnum(current_position));
    }

    EntityStateUpdate new_state;
    return new_state;
}

void SettingsApp::updateStateFromSystem(AppState state)
{
    page_mgr->getCurrentPage()->updateFromSystem(state);
}

void SettingsApp::handleNavigation(NavigationEvent event)
{
    page_mgr->getCurrentPage()->handleNavigation(event);
}

void SettingsApp::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    os_config_notifier_ = os_config_notifier;
    page_mgr = new SettingsPageManager(screen, mutex_, os_config_notifier_); // Should be created in the constructor moved here for fix of os_config_notifier_ being nullptr, UGLY
}