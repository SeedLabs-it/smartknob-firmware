#pragma once
#include "apps.h"
#include "menu.h"
#include "onboarding_menu.h"
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
    lock();
    this->spr_ = spr_;
    unlock();
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
            MenuItemOld{
                it->second->friendly_name,
                position, //! adding + 1 FIXES BUG WITH SYNC MIGHT CREATE MORE??
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
    ESP_LOGD("apps.cpp", "navigationNext: %d", millis());
    std::pair<app_types, uint8_t> next = active_app->navigationNext();
    navigation_history.push_back(next);
    unlock();
    return next;
}

std::pair<app_types, uint8_t> Apps::navigationBack()
{
    lock();

    if (navigation_history.size() == 0)
    {
        unlock();
        return std::make_pair(menu_type, 0); // SHOULD GO TO FIRST MENU/APP
    }

    std::pair<app_types, uint8_t> last = navigation_history.back();
    navigation_history.pop_back();

    unlock();
    return last;
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