#pragma once

#include <map>

// #include "lvgl_app.h"
// #include "menu.h"
#include "../app_config.h"
#include "../notify/motor_notifier/motor_notifier.h"
#include "../navigation/navigation.h"

// include all apps
// #include "apps/3d_printing_chamber/3d_printer_chamber.h"
// #include "apps/blinds/blinds.h"
// #include "apps/climate/climate.h"
#include "apps/light_dimmer/light_dimmer.h"
#include "apps/light_switch/light_switch.h"
// #include "apps/music/music.h"
// #include "apps/settings/settings.h"
// #include "apps/stopwatch/stopwatch.h"
// #include "apps/pomodoro/pomodoro.h"

// include all "menu" apps
#include "app_menu.h"

// TODO: generate menu based on items in the map

class Apps
{

public:
    // Apps() ;
    Apps(SemaphoreHandle_t mutex) : screen_mutex_(mutex)
    {
        app_mutex_ = xSemaphoreCreateMutex();
    }

    void add(uint8_t id, App *app)
    {
        SemaphoreGuard lock(app_mutex_);
        // lock();
        apps.insert(std::make_pair(id, app));
        // unlock();
    }

    void clear()
    {
        SemaphoreGuard lock(app_mutex_);
        // lock();
        apps.clear();
        // unlock();
    }

    EntityStateUpdate update(AppState state)
    {
        // TODO: update with AppState
        SemaphoreGuard lock(app_mutex_);
        EntityStateUpdate new_state_update;
        // lock();
        if (active_app == nullptr)
        {
            // unlock();
            return new_state_update;
        }

        new_state_update = active_app->updateStateFromKnob(state.motor_state);
        active_app->updateStateFromSystem(state);

        // unlock();
        return new_state_update;
    }
    // TFT_eSprite *renderActive();
    void renderActive()
    {
        active_app->render();
    };
    void setActive(int8_t id)
    {
        SemaphoreGuard lock(app_mutex_);
        // lock();
        if (id == MENU)
        {
            active_app = menu;
            active_id = MENU;
            // unlock();
            return;
        }
        LOGD("Set active %d", id);
        active_id = id;
        if (apps[active_id] == nullptr)
        {
            // TODO: panic?
            LOGW("Null pointer instead of app");
        }
        else
        {
            active_app = apps[active_id];
        }
        // unlock();
    }

    // void setSprite(TFT_eSprite *spr_);
    App *loadApp(uint8_t position, std::string app_slug, char *app_id, char *friendly_name, char *entity_id)
    {
        // if (app_slug.compare(APP_SLUG_CLIMATE) == 0)
        // {
        //     ClimateApp *app = new ClimateApp(this->spr_, app_id, friendly_name, entity_id);
        //     add(position, app);
        //     return app;
        // }
        // else if (app_slug.compare(APP_SLUG_3D_PRINTER) == 0)
        // {
        //     PrinterChamberApp *app = new PrinterChamberApp(this->spr_, app_id);
        //     // app->friendly_name = friendly_name;
        //     sprintf(app->friendly_name, "%s", friendly_name);
        //     add(position, app);
        //     return app;
        // }
        // else if (app_slug.compare(APP_SLUG_BLINDS) == 0)
        // {
        //     BlindsApp *app = new BlindsApp(this->spr_, app_id, friendly_name, entity_id);
        //     add(position, app);
        //     return app;
        // }
        if (app_slug.compare(APP_SLUG_LIGHT_DIMMER) == 0)
        {
            LightDimmerApp *app = new LightDimmerApp(screen_mutex_, app_id, friendly_name, entity_id);
            add(position, app);
            return app;
        }
        else if (app_slug.compare(APP_SLUG_LIGHT_SWITCH) == 0)
        {
            LightSwitchApp *app = new LightSwitchApp(screen_mutex_, app_id, friendly_name, entity_id);

            add(position, app);
            return app;
        }
        // else if (app_slug.compare(APP_SLUG_MUSIC) == 0)
        // {
        //     MusicApp *app = new MusicApp(this->spr_, app_id);
        //     // app->friendly_name = friendly_name;
        //     sprintf(app->friendly_name, "%s", friendly_name);
        //     add(position, app);
        //     return app;
        // }
        // else if (app_slug.compare(APP_SLUG_STOPWATCH) == 0)
        // {
        //     StopwatchApp *app = new StopwatchApp(this->spr_, app_id);
        //     // app->friendly_name = friendly_name;
        //     sprintf(app->friendly_name, "%s", friendly_name);
        //     add(position, app);
        //     return app;
        // }
        else
        {
            LOGW("Can't find app with slug '%s'", app_slug.c_str());
        }
        return nullptr;
    }
    void updateMenu() // BROKEN FOR NOW
    {
        // lock();
        SemaphoreGuard lock(app_mutex_);
        menu = std::make_shared<MenuApp>(screen_mutex_);

        std::map<uint8_t, std::shared_ptr<App>>::iterator it;

        uint16_t position = 0;

        // uint16_t active_color = spr_->color565(0, 255, 200);
        lv_color_t active_color = LV_COLOR_MAKE(0x00, 0xFF, 0xC8);
        // uint16_t inactive_color = spr_->color565(150, 150, 150);

        for (it = apps.begin(); it != apps.end(); it++)
        {
            menu->add_item(
                position,
                std::make_shared<MenuItem>(
                    (int8_t)it->first,
                    TextItem{it->second->friendly_name, active_color}, // TextItem{it->second->friendly_name, inactive_color},
                    TextItem{},
                    TextItem{},
                    IconItem{}, // IconItem{it->second->big_icon, active_color},
                    IconItem{}) // IconItem{it->second->small_icon, inactive_color}
            );

            position++;
        }

        // unlock();
        setActive(MENU);
    }

    void reload(cJSON *apps_);
    void createOnboarding();

    void setMotorNotifier(MotorNotifier *motor_notifier)
    {
        this->motor_notifier = motor_notifier;

        std::map<uint8_t, std::shared_ptr<App>>::iterator it;
        for (it = apps.begin(); it != apps.end(); it++)
        {
            it->second->setMotorNotifier(motor_notifier);
        }
    }
    void triggerMotorConfigUpdate()
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
                LOGW("No active app");
            }
        }
        else
        {
            LOGW("Motor_notifier is not set");
        }
    }
    void handleNavigationEvent(NavigationEvent event)
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

    PB_SmartKnobConfig blocked_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        PI,
        0,
        1,
        0.5,
        "",
        0,
        {},
        0,
        0,
    };

protected:
    SemaphoreHandle_t screen_mutex_;
    SemaphoreHandle_t app_mutex_;
    std::map<uint8_t, std::shared_ptr<App>> apps;
    std::shared_ptr<Menu> menu = nullptr;

    int8_t active_id = 0;

    // TFT_eSprite *spr_ = nullptr;
    std::shared_ptr<App> active_app = nullptr;

    // TFT_eSprite *rendered_spr_;

    std::shared_ptr<App> find(uint8_t id)
    {
        // TODO: add protection with array size
        return apps[id];
    }
    std::shared_ptr<App> find(char *app_id)
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

    PB_SmartKnobConfig root_level_motor_config;

    MotorNotifier *motor_notifier;
};
