#pragma once
#include "apps.h"
#include "menu.h"
#include "onboarding/onboarding_menu.h"
#include "app_menu.h"
#include "settings.h"

#include <typeinfo>
#include <iterator>

Apps::Apps()
{
    mutex = xSemaphoreCreateMutex();
}

void Apps::setSprite(TFT_eSprite *spr_)
{
    this->spr_ = spr_;
}

void Apps::add(uint8_t id, App *app)
{
    lock();
    apps.insert(std::make_pair(id, app));
    unlock();
}

void Apps::clear()
{
    lock();
    apps.clear();
    unlock();
}

EntityStateUpdate Apps::update(AppState state)
{
    // TODO: update with AppState
    lock();
    // TODO: check if active_app is not null
    EntityStateUpdate new_state_update = active_app->updateStateFromKnob(state.motor_state);
    active_app->updateStateFromSystem(state);

    unlock();
    return new_state_update;
}

TFT_eSprite *Apps::renderActive()
{
    // TODO: update with AppState
    lock();
    if (active_app != nullptr)
    {
        rendered_spr_ = active_app->render();
        unlock();
        return rendered_spr_;
    }

    //! MIGHT BE WRONG
    if (apps[active_id] == nullptr)
    {
        rendered_spr_ = spr_;
        ESP_LOGE("apps.cpp", "null pointer instead of app");
        unlock();
        return rendered_spr_;
    }

    active_app = apps[active_id];

    rendered_spr_ = active_app->render();

    unlock();
    return rendered_spr_;
}

void Apps::setActive(uint8_t id)
{
    lock();
    ESP_LOGD("apps.cpp", "set active %d", id);
    active_id = id;
    if (apps[active_id] == nullptr)
    {
        // TODO: panic?
        ESP_LOGE("apps.cpp", "null pointer instead of app");
        unlock();
    }
    else
    {
        active_app = apps[active_id];
        unlock();
    }
}

void Apps::reload(cJSON *apps_)
{
    clear();

    uint16_t app_position = 1;

    cJSON *json_app = NULL;
    cJSON_ArrayForEach(json_app, apps_)
    {
        cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app, "app_slug");
        cJSON *json_app_id = cJSON_GetObjectItemCaseSensitive(json_app, "app_id");
        cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app, "friendly_name");

        loadApp(app_position, std::string(json_app_slug->valuestring), json_app_id->valuestring, json_friendly_name->valuestring);

        app_position++;
    }

    SettingsApp *settings_app = new SettingsApp(this->spr_);
    add(app_position, settings_app);

    updateMenu();
    setActive(0);
    cJSON_Delete(apps_);
}

void Apps::createOnboarding()
{
    clear();

    OnboardingMenu *onboarding_menu = new OnboardingMenu(this->spr_);

    onboarding_menu->add_item(
        0,
        MenuItem{
            ONBOARDING_MENU,
            TextItem{"SMART KNOB", spr_->color565(255, 255, 255)},
            TextItem{"DEV KIT V0.1", spr_->color565(255, 255, 255)},
            TextItem{"ROTATE TO START", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{},
        });

    onboarding_menu->add_item(
        1,
        MenuItem{
            HASS_SETUP_APP,
            TextItem{"HOME ASSISTANT", spr_->color565(255, 255, 255)},
            TextItem{"INTEGRATION", spr_->color565(255, 255, 255)},
            TextItem{"PRESS TO CONTINUE", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{home_assistant_80, spr_->color565(17, 189, 242)},
        });

    onboarding_menu->add_item(
        2,
        MenuItem{
            SETTINGS,
            TextItem{"WIFI", spr_->color565(255, 255, 255)},
            TextItem{},
            TextItem{"PRESS TO CONFIGURE", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{wifi_conn_80, spr_->color565(255, 255, 255)},
        });
    onboarding_menu->add_item(
        3,
        MenuItem{
            APP_MENU,
            TextItem{"DEMO MODE", spr_->color565(255, 255, 255)},
            TextItem{},
            TextItem{"PRESS TO START", spr_->color565(128, 255, 80)},
            IconItem{},
            IconItem{},
        });
    onboarding_menu->add_item(
        4,
        MenuItem{
            ONBOARDING_MENU,
            TextItem{"FIRMWARE 0.1b", spr_->color565(255, 255, 255)},
            TextItem{"HARDWARE: DEVKIT V0.1", spr_->color565(255, 255, 255)},
            TextItem{"SEEDLABS.IT ®", spr_->color565(255, 255, 255)}, // TODO "®" doesnt show up
            IconItem{},
            IconItem{},

        });

    add(ONBOARDING_MENU, onboarding_menu);

    // APPS FOR OTHER ONBOARDING SCREENS
    HassSetupApp *hass_setup_app = new HassSetupApp(spr_);
    add(HASS_SETUP_APP, hass_setup_app);

    StopwatchApp *app1 = new StopwatchApp(spr_, "");
    add(STOPWATCH, app1);

    // INIT DEMO MENU
    uint16_t active_color = spr_->color565(0, 255, 200);
    uint16_t inactive_color = spr_->color565(150, 150, 150);

    MenuApp *menu_app = new MenuApp(spr_);
    SettingsApp *settings_app = new SettingsApp(spr_);
    add(SETTINGS, settings_app);

    menu_app->add_item(
        0,
        MenuItem{
            SETTINGS,
            TextItem{"SETTINGS", inactive_color},
            TextItem{},
            TextItem{"SETTINGS", inactive_color},
            IconItem{settings_80, active_color},
            IconItem{settings_40, inactive_color},

        });

    StopwatchApp *stopwatch_app = new StopwatchApp(spr_, "");
    add(STOPWATCH, stopwatch_app);

    menu_app->add_item(
        1,
        MenuItem{
            STOPWATCH,
            TextItem{"STOPWATCH", inactive_color},
            TextItem{},
            TextItem{"STOPWATCH", spr_->color565(128, 255, 80)},
            IconItem{stopwatch_app->big_icon, active_color},
            IconItem{stopwatch_app->small_icon, inactive_color},
        });

    LightSwitchApp *light_switch_app = new LightSwitchApp(spr_, "light.ceiling", "Ceiling");
    add(LIGHT_SWITCH, light_switch_app);

    menu_app->add_item(
        2,
        MenuItem{
            LIGHT_SWITCH,
            TextItem{"CEILING", inactive_color},
            TextItem{},
            TextItem{"CEILING", inactive_color},
            IconItem{light_switch_app->big_icon, active_color},
            IconItem{light_switch_app->small_icon, inactive_color},
        });

    LightDimmerApp *light_dimmer_app = new LightDimmerApp(spr_, "light.workbench", "Workbench");
    add(LIGHT_DIMMER, light_dimmer_app);

    menu_app->add_item(
        3,
        MenuItem{
            LIGHT_DIMMER,
            TextItem{"WORKBENCH", inactive_color},
            TextItem{},
            TextItem{"WORKBENCH", inactive_color},
            IconItem{light_dimmer_app->big_icon, active_color},
            IconItem{light_dimmer_app->small_icon, inactive_color},
        });

    ClimateApp *climate_app = new ClimateApp(spr_, "climate.office");
    add(CLIMATE, climate_app);

    menu_app->add_item(
        4,
        MenuItem{
            CLIMATE,
            TextItem{"CLIMATE", inactive_color},
            TextItem{},
            TextItem{"CLIMATE", inactive_color},
            IconItem{climate_app->big_icon, active_color},
            IconItem{climate_app->small_icon, inactive_color},
        });

    PrinterChamberApp *printer_chamber_app = new PrinterChamberApp(spr_, "3d_printer.office");
    add(PRINTER_CHAMBER, printer_chamber_app);

    menu_app->add_item(
        5,
        MenuItem{
            PRINTER_CHAMBER,
            TextItem{"3D PRINTER", inactive_color},
            TextItem{},
            TextItem{"3D PRINTER", inactive_color},
            IconItem{printer_chamber_app->big_icon, active_color},
            IconItem{printer_chamber_app->small_icon, inactive_color},
        });

    BlindsApp *blinds_app = new BlindsApp(spr_, "blinds.office");
    add(BLINDS, blinds_app);

    menu_app->add_item(
        6,
        MenuItem{
            BLINDS,
            TextItem{"BLINDS", inactive_color},
            TextItem{},
            TextItem{"BLINDS", active_color},
            IconItem{blinds_app->big_icon, active_color},
            IconItem{blinds_app->small_icon, inactive_color},
        });

    MusicApp *music_app = new MusicApp(spr_, "music.office");
    add(MUSIC, music_app);

    menu_app->add_item(
        7,
        MenuItem{
            MUSIC,
            TextItem{"MUSIC", inactive_color},
            TextItem{},
            TextItem{"MUSIC", active_color},
            IconItem{music_app->big_icon, active_color},
            IconItem{music_app->small_icon, inactive_color},
        });

    add(APP_MENU, menu_app);

    setActive(0);
}

void Apps::updateMenu() // BROKEN FOR NOW
{
    // re - generate new menu based on loaded apps
    MenuApp *menu_app = new MenuApp(spr_);

    std::map<uint8_t, std::shared_ptr<App>>::iterator it;

    uint16_t position = 0;

    for (it = apps.begin(); it != apps.end(); it++)
    {
        ESP_LOGD("apps.cpp", "menu add item %d", position);

        menu_app->add_item(
            position,
            MenuItem{
                0,
                TextItem{
                    it->second->friendly_name,
                    spr_->color565(255, 255, 255),
                },
                TextItem{
                    "",
                    spr_->color565(255, 255, 255),
                },
                TextItem{
                    it->second->friendly_name,
                    spr_->color565(128, 255, 80),
                },
                IconItem{
                    it->second->big_icon,
                    spr_->color565(255, 255, 255),
                },
                IconItem{
                    it->second->small_icon,
                    spr_->color565(255, 255, 255),
                },
            });

        position++;
    }

    add(APP_MENU, menu_app);
}

// settings and menu apps kept aside for a reason. We will add them manually later
App *Apps::loadApp(uint8_t position, std::string app_slug, std::string app_id, std::string friendly_name)
{
    if (position < 1)
    {
        ESP_LOGE("apps.cpp", "can't load app at %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
    }

    ESP_LOGD("apps.cpp", "loading app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
    if (app_slug.compare(APP_SLUG_CLIMATE) == 0)
    {
        ClimateApp *app = new ClimateApp(this->spr_, app_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_3D_PRINTER) == 0)
    {
        PrinterChamberApp *app = new PrinterChamberApp(this->spr_, app_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_BLINDS) == 0)
    {
        BlindsApp *app = new BlindsApp(this->spr_, app_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_DIMMER) == 0)
    {
        LightDimmerApp *app = new LightDimmerApp(this->spr_, app_id, friendly_name);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_SWITCH) == 0)
    {
        LightSwitchApp *app = new LightSwitchApp(this->spr_, app_id, friendly_name);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_MUSIC) == 0)
    {
        MusicApp *app = new MusicApp(this->spr_, app_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_STOPWATCH) == 0)
    {
        StopwatchApp *app = new StopwatchApp(this->spr_, app_id);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else
    {
        ESP_LOGE("apps.cpp", "can't find app with slug '%s'", app_slug);
    }
    return nullptr;
}

uint8_t Apps::navigationNext()
{
    return active_app->next;
}

uint8_t Apps::navigationBack()
{
    return active_app->back;
}

PB_SmartKnobConfig Apps::getActiveMotorConfig()
{
    lock();
    // TODO MAYBE CHECK IF ACTIVE APP IS NOT NULL
    PB_SmartKnobConfig motor_config = active_app->getMotorConfig();

    unlock();
    return motor_config;
}

std::shared_ptr<App> Apps::find(uint8_t id)
{
    // TODO: add protection with array size
    return apps[id];
}

void Apps::lock()
{
    xSemaphoreTake(mutex, portMAX_DELAY);
}
void Apps::unlock()
{
    xSemaphoreGive(mutex);
}