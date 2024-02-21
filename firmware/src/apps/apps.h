#pragma once

#include <map>

#include "app.h"
#include "menu.h"
#include "../app_config.h"
#include "../notify/motor_notifier/motor_notifier.h"
#include "../navigation/navigation.h"

// include all apps
#include "apps/3d_printing_chamber/3d_printer_chamber.h"
#include "apps/blinds/blinds.h"
#include "apps/climate/climate.h"
#include "apps/light_dimmer/light_dimmer.h"
#include "apps/light_switch/light_switch.h"
#include "apps/music/music.h"
#include "apps/settings/settings.h"
#include "apps/stopwatch/stopwatch.h"
#include "apps/pomodoro/pomodoro.h"

// include all "menu" apps
#include "app_menu.h"

// TODO: generate menu based on items in the map

class Apps
{

public:
    Apps();
    Apps(TFT_eSprite *spr_);
    void add(uint8_t id, App *app);
    void clear();
    EntityStateUpdate update(AppState state);
    TFT_eSprite *renderActive();
    void setActive(int8_t id);

    void setSprite(TFT_eSprite *spr_);
    App *loadApp(uint8_t position, std::string app_slug, char *app_id, char *friendly_name);
    void updateMenu();

    void reload(cJSON *apps_);
    void createOnboarding();

    void setMotorNotifier(MotorNotifier *motor_notifier);
    void triggerMotorConfigUpdate();

    void handleNavigationEvent(NavigationEvent event);

protected:
    QueueHandle_t mutex;
    std::map<uint8_t, std::shared_ptr<App>> apps;
    std::shared_ptr<Menu> menu = nullptr;

    uint8_t active_id = 0;

    TFT_eSprite *spr_ = nullptr;
    std::shared_ptr<App> active_app = nullptr;

    TFT_eSprite *rendered_spr_;

    std::shared_ptr<App> find(uint8_t id);
    std::shared_ptr<App> find(char *app_slug);
    void lock();
    void unlock();

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config;

    MotorNotifier *motor_notifier;
};
