#pragma once
#include "apps.h"
#include "menu.h"
#include "app_menu.h"
#include "settings.h"

#include <typeinfo>
#include <iterator>

Apps::Apps()
{
    mutex = xSemaphoreCreateMutex();
}

Apps::Apps(TFT_eSprite *spr_)
{
    mutex = xSemaphoreCreateMutex();
    this->spr_ = spr_;
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

void Apps::setActive(int8_t id)
{
    lock();
    if (id == MENU)
    {
        active_app = menu;
        unlock();
        return;
    }
    ESP_LOGD("apps.cpp", "set active %d", id);
    active_id = id;
    if (apps[active_id] == nullptr)
    {
        // TODO: panic?
        ESP_LOGE("apps.cpp", "null pointer instead of app");
    }
    else
    {
        active_app = apps[active_id];
    }
    unlock();
}

void Apps::updateMenu() // BROKEN FOR NOW
{
    lock();
    menu = std::make_shared<MenuApp>(spr_);

    std::map<uint8_t, std::shared_ptr<App>>::iterator it;

    uint16_t position = 0;

    uint16_t active_color = spr_->color565(0, 255, 200);
    uint16_t inactive_color = spr_->color565(150, 150, 150);

    for (it = apps.begin(); it != apps.end(); it++)
    {
        menu->add_item(
            position,
            std::make_shared<MenuItem>(
                (int8_t)it->first,
                TextItem{it->second->friendly_name, inactive_color},
                TextItem{},
                TextItem{},
                IconItem{it->second->big_icon, active_color},
                IconItem{it->second->small_icon, inactive_color}));

        position++;
    }
    unlock();
    setActive(MENU);
}

// settings and menu apps kept aside for a reason. We will add them manually later
App *Apps::loadApp(uint8_t position, std::string app_slug, char *app_id, char *friendly_name)
{

    ESP_LOGD("apps.cpp", "loading app %d %s %s %s", position, app_slug, app_id, friendly_name);
    if (app_slug.compare(APP_SLUG_CLIMATE) == 0)
    {
        ClimateApp *app = new ClimateApp(this->spr_, app_id);
        app->friendly_name = friendly_name;
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_3D_PRINTER) == 0)
    {
        PrinterChamberApp *app = new PrinterChamberApp(this->spr_, app_id);
        app->friendly_name = friendly_name;
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_BLINDS) == 0)
    {
        BlindsApp *app = new BlindsApp(this->spr_, app_id);
        app->friendly_name = friendly_name;
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_DIMMER) == 0)
    {
        LightDimmerApp *app = new LightDimmerApp(this->spr_, app_id, friendly_name);
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_SWITCH) == 0)
    {
        LightSwitchApp *app = new LightSwitchApp(this->spr_, app_id, friendly_name);

        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_MUSIC) == 0)
    {
        MusicApp *app = new MusicApp(this->spr_, app_id);
        app->friendly_name = friendly_name;
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_STOPWATCH) == 0)
    {
        StopwatchApp *app = new StopwatchApp(this->spr_, app_id);
        app->friendly_name = friendly_name;
        add(position, app);
        ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
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
    return active_app->navigationNext();
}

uint8_t Apps::navigationBack()
{
    return active_app->navigationBack();
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