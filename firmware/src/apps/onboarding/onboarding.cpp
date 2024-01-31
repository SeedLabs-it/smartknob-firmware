#include "onboarding.h"

Onboarding::Onboarding(TFT_eSprite *spr_) : Apps(spr_)
{
    menu = std::make_shared<OnboardingMenu>(this->spr_);

    OnboardingMenu *onboarding_menu = new OnboardingMenu(spr_);

    menu->add_item(
        0,
        std::make_shared<MenuItem>(
            DONT_NAVIGATE,
            TextItem{"SMART KNOB", spr_->color565(255, 255, 255)},
            TextItem{"DEV KIT V0.1", spr_->color565(255, 255, 255)},
            TextItem{"ROTATE TO START", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{}));

    menu->add_item(
        1,
        std::make_shared<MenuItem>(
            HASS_SETUP_APP,
            TextItem{"HOME ASSISTANT", spr_->color565(255, 255, 255)},
            TextItem{"INTEGRATION", spr_->color565(255, 255, 255)},
            TextItem{"PRESS TO CONTINUE", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{home_assistant_80, spr_->color565(17, 189, 242)}));

    menu->add_item(
        2,
        std::make_shared<MenuItem>(
            DONT_NAVIGATE,
            TextItem{"WIFI", spr_->color565(255, 255, 255)},
            TextItem{},
            TextItem{"PRESS TO CONFIGURE", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{wifi_conn_80, spr_->color565(255, 255, 255)}));
    menu->add_item(
        3,
        std::make_shared<MenuItem>(
            DEMO,
            TextItem{"DEMO MODE", spr_->color565(255, 255, 255)},
            TextItem{},
            TextItem{"PRESS TO START", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{}));
    menu->add_item(
        4,
        std::make_shared<MenuItem>(
            DONT_NAVIGATE,
            TextItem{"FIRMWARE 0.1b", spr_->color565(255, 255, 255)},
            TextItem{"HARDWARE: DEVKIT V0.1", spr_->color565(255, 255, 255)},
            TextItem{"SEEDLABS.IT ®", spr_->color565(255, 255, 255)}, // TODO "®" doesnt show up
            IconItem{},
            IconItem{}));

    // APPS FOR OTHER ONBOARDING SCREENS
    HassSetupApp *hass_setup_app = new HassSetupApp(spr_);
    add(HASS_SETUP_APP, hass_setup_app);

    // INIT DEMO MENU
    // MOVE COLORS TO CORRESPONDING MENU
    uint16_t active_color = spr_->color565(0, 255, 200);
    uint16_t inactive_color = spr_->color565(150, 150, 150);

    MenuApp *menu_app = new MenuApp(spr_);
    menu_app->setBack(MENU); // TEMP FIXES FOR DEMO MODE
    SettingsApp *settings_app = new SettingsApp(spr_);
    settings_app->setBack(DEMO); // TEMP FIXES FOR DEMO MODE
    add(SETTINGS, settings_app);

    menu_app->add_item(
        0,
        std::make_shared<MenuItem>(
            SETTINGS,
            TextItem{"SETTINGS", inactive_color},
            TextItem{},
            TextItem{"SETTINGS", inactive_color},
            IconItem{settings_80, active_color},
            IconItem{settings_40, inactive_color}));

    StopwatchApp *stopwatch_app = new StopwatchApp(spr_, "");
    stopwatch_app->setBack(DEMO);
    add(STOPWATCH, stopwatch_app);

    menu_app->add_item(
        1,
        std::make_shared<MenuItem>(
            STOPWATCH,
            TextItem{"STOPWATCH", inactive_color},
            TextItem{},
            TextItem{"STOPWATCH", spr_->color565(128, 255, 80)},
            IconItem{stopwatch_app->big_icon, active_color},
            IconItem{stopwatch_app->small_icon, inactive_color}));

    LightSwitchApp *light_switch_app = new LightSwitchApp(spr_, "light.ceiling", "Ceiling");
    light_switch_app->setBack(DEMO);
    add(LIGHT_SWITCH, light_switch_app);

    menu_app->add_item(
        2,
        std::make_shared<MenuItem>(
            LIGHT_SWITCH,
            TextItem{"CEILING", inactive_color},
            TextItem{},
            TextItem{"CEILING", inactive_color},
            IconItem{light_switch_app->big_icon, active_color},
            IconItem{light_switch_app->small_icon, inactive_color}));

    LightDimmerApp *light_dimmer_app = new LightDimmerApp(spr_, "light.workbench", "Workbench");
    light_dimmer_app->setBack(DEMO);
    add(LIGHT_DIMMER, light_dimmer_app);

    menu_app->add_item(
        3,
        std::make_shared<MenuItem>(
            LIGHT_DIMMER,
            TextItem{"WORKBENCH", inactive_color},
            TextItem{},
            TextItem{"WORKBENCH", inactive_color},
            IconItem{light_dimmer_app->big_icon, active_color},
            IconItem{light_dimmer_app->small_icon, inactive_color}));

    ClimateApp *climate_app = new ClimateApp(spr_, "climate.office");
    climate_app->setBack(DEMO);
    add(CLIMATE, climate_app);

    menu_app->add_item(
        4,
        std::make_shared<MenuItem>(
            CLIMATE,
            TextItem{"CLIMATE", inactive_color},
            TextItem{},
            TextItem{"CLIMATE", inactive_color},
            IconItem{climate_app->big_icon, active_color},
            IconItem{climate_app->small_icon, inactive_color}));

    PrinterChamberApp *printer_chamber_app = new PrinterChamberApp(spr_, "3d_printer.office");
    printer_chamber_app->setBack(DEMO);
    add(PRINTER_CHAMBER, printer_chamber_app);

    menu_app->add_item(
        5,
        std::make_shared<MenuItem>(
            PRINTER_CHAMBER,
            TextItem{"3D PRINTER", inactive_color},
            TextItem{},
            TextItem{"3D PRINTER", inactive_color},
            IconItem{printer_chamber_app->big_icon, active_color},
            IconItem{printer_chamber_app->small_icon, inactive_color}));

    BlindsApp *blinds_app = new BlindsApp(spr_, "blinds.office");
    blinds_app->setBack(DEMO);
    add(BLINDS, blinds_app);

    menu_app->add_item(
        6,
        std::make_shared<MenuItem>(
            BLINDS,
            TextItem{"BLINDS", inactive_color},
            TextItem{},
            TextItem{"BLINDS", active_color},
            IconItem{blinds_app->big_icon, active_color},
            IconItem{blinds_app->small_icon, inactive_color}));

    MusicApp *music_app = new MusicApp(spr_, "music.office");
    music_app->setBack(DEMO);
    add(MUSIC, music_app);

    menu_app->add_item(
        7,
        std::make_shared<MenuItem>(
            MUSIC,
            TextItem{"MUSIC", inactive_color},
            TextItem{},
            TextItem{"MUSIC", active_color},
            IconItem{music_app->big_icon, active_color},
            IconItem{music_app->small_icon, inactive_color}));

    add(DEMO, menu_app);
    setActive(MENU);
}