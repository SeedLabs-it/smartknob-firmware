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

Apps::Apps(TFT_eSprite *spr_, TFT_eSPI *tft_)
{
    mutex = xSemaphoreCreateMutex();
    this->spr_ = spr_;
    this->tft_ = tft_;
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
    EntityStateUpdate new_state_update;
    lock();
    if (active_app == nullptr)
    {
        unlock();
        return new_state_update;
    }

    new_state_update = active_app->updateStateFromKnob(state.motor_state);
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
        active_id = MENU;
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
// TODO: create struct for data passed to each app.
App *Apps::loadApp(uint8_t position, std::string app_slug, char *app_id, char *friendly_name, char *entity_id)
{
    // ESP_LOGD("apps.cpp", "loading app %d %s %s %s", position, app_slug, app_id, friendly_name);
    if (app_slug.compare(APP_SLUG_CLIMATE) == 0)
    {
        ClimateApp *app = new ClimateApp(this->spr_, app_id, friendly_name, entity_id);
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_3D_PRINTER) == 0)
    {
        PrinterChamberApp *app = new PrinterChamberApp(this->spr_, app_id);
        // app->friendly_name = friendly_name;
        sprintf(app->friendly_name, "%s", friendly_name);
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_BLINDS) == 0)
    {
        BlindsApp *app = new BlindsApp(this->spr_, app_id, friendly_name, entity_id);
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_DIMMER) == 0)
    {
        LightDimmerApp *app = new LightDimmerApp(this->spr_, this->tft_, app_id, friendly_name, entity_id);
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_LIGHT_SWITCH) == 0)
    {
        LightSwitchApp *app = new LightSwitchApp(this->spr_, app_id, friendly_name, entity_id);

        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_MUSIC) == 0)
    {
        MusicApp *app = new MusicApp(this->spr_, app_id);
        // app->friendly_name = friendly_name;
        sprintf(app->friendly_name, "%s", friendly_name);
        add(position, app);
        // ESP_LOGD("apps.cpp", "added app %d %s %s %s", position, app_slug, app_id, friendly_name);
        return app;
    }
    else if (app_slug.compare(APP_SLUG_STOPWATCH) == 0)
    {
        StopwatchApp *app = new StopwatchApp(this->spr_, app_id);
        // app->friendly_name = friendly_name;
        sprintf(app->friendly_name, "%s", friendly_name);
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
        if (active_app != nullptr)
        {
            motor_notifier->requestUpdate(active_app->getMotorConfig());
        }
        else
        {
            motor_notifier->requestUpdate(blocked_motor_config);
            ESP_LOGE("onboarding_flow", "no active app");
        }
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