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

#ifndef USE_DISPLAY_BUFFER
Apps::Apps(TFT_eSprite *spr_)
{
    mutex = xSemaphoreCreateMutex();
    this->spr_ = spr_;
}
#endif

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

#ifdef USE_DISPLAY_BUFFER
void Apps::render()
#else
TFT_eSprite *Apps::renderActive()
#endif
{
    // TODO: update with AppState
    lock();
    if (active_app != nullptr)
    {
        #ifdef USE_DISPLAY_BUFFER
            active_app->render();
        #else
            rendered_spr_ = active_app->render();
        #endif
        unlock();
        #ifndef USE_DISPLAY_BUFFER
            return rendered_spr_;
        #endif
    }

    //! MIGHT BE WRONG
    if (apps[active_id] == nullptr)
    {
        rendered_spr_ = spr_;
        ESP_LOGE("apps.cpp", "null pointer instead of app");
        unlock();
        #ifndef USE_DISPLAY_BUFFER
            return rendered_spr_;
        #endif
    }

    active_app = apps[active_id];

    #ifdef USE_DISPLAY_BUFFER
        active_app->render();
    #else
        rendered_spr_ = active_app->render();
    #endif

    unlock();
    #ifndef USE_DISPLAY_BUFFER
        return rendered_spr_;
    #endif
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
    #ifdef USE_DISPLAY_BUFFER
    menu = std::make_shared<MenuApp>();
    #else
    menu = std::make_shared<MenuApp>(spr_);
    #endif

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

    // ESP_LOGD("apps.cpp", "loading app %d %s %s %s", position, app_slug, app_id, friendly_name);
    if (app_slug.compare(APP_SLUG_CLIMATE) == 0)
    {
        #ifdef USE_DISPLAY_BUFFER
        ClimateApp *app = new ClimateApp(app_id);
        #else
        ClimateApp *app = new ClimateApp(this->spr_, app_id);
        #endif
        app->friendly_name = friendly_name;
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_3D_PRINTER) == 0)
    {   
        #ifdef USE_DISPLAY_BUFFER
        PrinterChamberApp *app = new PrinterChamberApp(app_id);
        #else
        PrinterChamberApp *app = new PrinterChamberApp(this->spr_, app_id);
        #endif
        app->friendly_name = friendly_name;
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_BLINDS) == 0)
    {
        #ifdef USE_DISPLAY_BUFFER
        BlindsApp *app = new BlindsApp(app_id);
        #else
        BlindsApp *app = new BlindsApp(this->spr_, app_id);
        #endif
        app->friendly_name = friendly_name;
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_DIMMER) == 0)
    {
        #ifdef USE_DISPLAY_BUFFER
        LightDimmerApp *app = new LightDimmerApp();
        #else
        LightDimmerApp *app = new LightDimmerApp(this->spr_, app_id, friendly_name);
        #endif
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_SWITCH) == 0)
    {
        #ifdef USE_DISPLAY_BUFFER
        LightSwitchApp *app = new LightSwitchApp(app_id, friendly_name);
        #else
        LightSwitchApp *app = new LightSwitchApp(this->spr_, app_id, friendly_name);
        #endif

        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_MUSIC) == 0)
    {
        #ifdef USE_DISPLAY_BUFFER
        MusicApp *app = new MusicApp(app_id);
        #else
        MusicApp *app = new MusicApp(this->spr_, app_id);
        #endif
        app->friendly_name = friendly_name;
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_STOPWATCH) == 0)
    {
        #ifdef USE_DISPLAY_BUFFER
        StopwatchApp *app = new StopwatchApp(app_id);
        #else
        StopwatchApp *app = new StopwatchApp(this->spr_, app_id);
        #endif
        app->friendly_name = friendly_name;
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else
    {
        ESP_LOGE("apps.cpp", "can't find app with slug '%s'", app_slug);
    }
    return nullptr;
}

void Apps::handleNavigationEvent(NavigationEvent event)
{
    if (event.press == NAVIGATION_EVENT_PRESS_SHORT)
    {
        switch (active_app->navigationNext())
        {
        case DONT_NAVIGATE:
            return;
            break;
        case DONT_NAVIGATE_UPDATE_MOTOR_CONFIG:
            break;
        default:
            setActive(active_app->navigationNext());
            break;
        }
        motor_notifier->requestUpdate(active_app->getMotorConfig());
    }

    if (event.press == NAVIGATION_EVENT_PRESS_LONG)
    {
        switch (active_app->navigationBack())
        {
        case DONT_NAVIGATE:
            return;
            break;
        case DONT_NAVIGATE_UPDATE_MOTOR_CONFIG:
            break;
        default:
            setActive(active_app->navigationBack());
            break;
        }
        motor_notifier->requestUpdate(active_app->getMotorConfig());
    }
}

std::shared_ptr<App> Apps::find(uint8_t id)
{
    // TODO: add protection with array size
    return apps[id];
}

std::shared_ptr<App> Apps::find(char *app_id)
{
    std::map<uint8_t, std::shared_ptr<App>>::iterator it;
    for (it = apps.begin(); it != apps.end(); it++)
    {
        if (strcmp(it->second->app_id, app_id) == 0)
        {
            return it->second;
        }
    }
    return nullptr;
}

void Apps::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;

    std::map<uint8_t, std::shared_ptr<App>>::iterator it;
    for (it = apps.begin(); it != apps.end(); it++)
    {
        it->second->setMotorNotifier(motor_notifier);
    }
}

void Apps::triggerMotorConfigUpdate()
{
    if (this->motor_notifier != nullptr)
    {
        motor_notifier->requestUpdate(active_app->getMotorConfig());
    }
    else
    {
        ESP_LOGE("onboarding_flow", "motor_notifier is not set");
    }
}

void Apps::lock()
{
    xSemaphoreTake(mutex, portMAX_DELAY);
}
void Apps::unlock()
{
    xSemaphoreGive(mutex);
}