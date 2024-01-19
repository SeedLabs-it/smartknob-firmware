#pragma once
#include "apps.h"
#include "menu.h"
#include "onboarding.h"
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

void Apps::add(app_types type, uint8_t id, App *app)
{
    lock();
    apps[type].insert(std::make_pair(id, app));
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
    if (apps[apps_type][active_id] == nullptr)
    {
        rendered_spr_ = spr_;
        ESP_LOGE("apps.cpp", "null pointer instead of app");
        unlock();
        return rendered_spr_;
    }

    active_app = apps[apps_type][active_id];

    rendered_spr_ = active_app->render();

    unlock();
    return rendered_spr_;
}

void Apps::setActive(app_types type, uint8_t id)
{
    lock();
    active_id = id;
    if (apps[type][active_id] == nullptr)
    {
        // TODO: panic?
        ESP_LOGE("apps.cpp", "null pointer instead of app");
        unlock();
    }
    else
    {
        active_app = apps[type][active_id];
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
    add(apps_type, app_position, settings_app);

    updateMenu();
    setActive(menu_type, 0);
    cJSON_Delete(apps_);
}

void Apps::createOnboarding()
{
    clear();

    OnboardingApp *onboarding_app = new OnboardingApp(this->spr_);

    onboarding_app->add_item(
        0,
        OnboardingItem{
            1,
            TextItem{
                "SMART KNOB",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "DEV KIT V0.1",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "ROTATE TO START",
                spr_->color565(128, 255, 80),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },
        });

    onboarding_app->add_item(
        1,
        OnboardingItem{
            2,
            TextItem{
                "HOME ASSISTANT",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "INTEGRATION",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "PRESS TO CONTINUE",
                spr_->color565(128, 255, 80),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },
            IconItem{
                home_assistant_80,
                spr_->color565(17, 189, 242),
            },
        });

    onboarding_app->add_item(
        2,
        OnboardingItem{
            3,
            TextItem{
                "WIFI",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "PRESS TO CONFIGURE",
                spr_->color565(128, 255, 80),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },
            IconItem{
                wifi_conn_80,
                spr_->color565(255, 255, 255),
            },
        });
    onboarding_app->add_item(
        3,
        OnboardingItem{
            4,
            TextItem{
                "DEMO MODE",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "PRESS TO START",
                spr_->color565(128, 255, 80),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },
        });
    onboarding_app->add_item(
        4,
        OnboardingItem{
            5,
            TextItem{
                "FIRMWARE 0.1b",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "HARDWARE: DEVKIT V0.1",
                spr_->color565(255, 255, 255),
            },
            TextItem{
                "SEEDLABS.IT ®", // TODO "®" doesnt show up
                spr_->color565(255, 255, 255),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },
            IconItem{
                nullptr,
                spr_->color565(255, 255, 255),
            },

        });

    add(menu_type, 0, onboarding_app);

    // APPS FOR OTHER ONBOARDING SCREENS
    HassSetupApp *hass_setup_app = new HassSetupApp(spr_);
    add(apps_type, 1, hass_setup_app);

    StopwatchApp *app1 = new StopwatchApp(spr_, "");
    add(apps_type, 2, app1);

    // FOR DEMO
    MenuApp *menu_app = new MenuApp(spr_);
    SettingsApp *settings_app = new SettingsApp(spr_);
    add(apps_type, 4, settings_app);

    menu_app->add_item(
        0,
        MenuItem{
            "SETTINGS",
            4,
            spr_->color565(0, 255, 200),
            settings_40,
            settings_80,
        });

    std::string apps_config = "[{\"app_slug\":\"stopwatch\",\"app_id\":\"stopwatch.office\",\"friendly_name\":\"Stopwatch\",\"area\":\"office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_switch\",\"app_id\":\"light.ceiling\",\"friendly_name\":\"Ceiling\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_dimmer\",\"app_id\":\"light.workbench\",\"friendly_name\":\"Workbench\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"thermostat\",\"app_id\":\"climate.office\",\"friendly_name\":\"Climate\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"3d_printer\",\"app_id\":\"3d_printer.office\",\"friendly_name\":\"3D Printer\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"blinds\",\"app_id\":\"blinds.office\",\"friendly_name\":\"Shades\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"music\",\"app_id\":\"music.office\",\"friendly_name\":\"Music\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"}]";

    cJSON *json_root = cJSON_Parse(apps_config.c_str());
    cJSON *json_app = NULL;

    uint16_t app_position = 5;
    uint16_t menu_position = 1;

    cJSON_ArrayForEach(json_app, json_root)
    {
        cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app, "app_slug");
        cJSON *json_app_id = cJSON_GetObjectItemCaseSensitive(json_app, "app_id");
        cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app, "friendly_name");

        App *app = loadApp(app_position, std::string(json_app_slug->valuestring), std::string(json_app_id->valuestring), json_friendly_name->valuestring);

        menu_app->add_item(
            menu_position,
            MenuItem{
                app->friendly_name,
                app_position,
                spr_->color565(0, 255, 200),
                app->small_icon,
                app->big_icon,
            });

        app_position++;
        menu_position++;
    }

    add(menu_type, 1, menu_app);

    setActive(menu_type, 0);
}

void Apps::updateMenu()
{
    // re - generate new menu based on loaded apps
    MenuApp *menu_app = new MenuApp(spr_);

    std::map<uint8_t, std::shared_ptr<App>>::iterator it;

    uint16_t position = 0;

    for (it = apps[apps_type].begin(); it != apps[apps_type].end(); it++)
    {
        ESP_LOGD("apps.cpp", "menu add item %d", position);

        menu_app->add_item(
            position,
            MenuItem{
                it->second->friendly_name,
                position + 1, //! FIXES BUG WITH SYNC MIGHT CREATE MORE??
                spr_->color565(0, 255, 200),
                it->second->small_icon,
                it->second->big_icon,
            });

        position++;
    }

    add(menu_type, 0, menu_app);
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
        add(apps_type, position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_3D_PRINTER) == 0)
    {
        PrinterChamberApp *app = new PrinterChamberApp(this->spr_, app_id);
        add(apps_type, position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_BLINDS) == 0)
    {
        BlindsApp *app = new BlindsApp(this->spr_, app_id);
        add(apps_type, position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_DIMMER) == 0)
    {
        LightDimmerApp *app = new LightDimmerApp(this->spr_, app_id, friendly_name);
        add(apps_type, position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_SWITCH) == 0)
    {
        LightSwitchApp *app = new LightSwitchApp(this->spr_, app_id, friendly_name);
        add(apps_type, position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_MUSIC) == 0)
    {
        MusicApp *app = new MusicApp(this->spr_, app_id);
        add(apps_type, position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_STOPWATCH) == 0)
    {
        StopwatchApp *app = new StopwatchApp(this->spr_, app_id);
        add(apps_type, position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug.c_str(), app_id.c_str(), friendly_name);
        return app;
    }
    else
    {
        ESP_LOGE("apps.cpp", "can't find app with slug '%s'", app_slug);
    }
    return nullptr;
}

std::pair<app_types, uint8_t> Apps::navigationNext()
{
    lock();
    // TODO MAYBE CHECK IF ACTIVE APP IS NOT NULL
    std::pair<app_types, uint8_t> next = active_app->navigationNext();
    unlock();
    return std::make_pair(next.first, next.second);
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
    return apps[apps_type][id];
}

void Apps::lock()
{
    xSemaphoreTake(mutex, portMAX_DELAY);
}
void Apps::unlock()
{
    xSemaphoreGive(mutex);
}