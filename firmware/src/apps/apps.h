#pragma once

#include <map>

#include "app.h"
#include "menu.h"
#include "../app_config.h"

// include all apps
#include "apps/3d_printing_chamber/3d_printer_chamber.h"
#include "apps/blinds/blinds.h"
#include "apps/climate/climate.h"
#include "apps/light_dimmer/light_dimmer.h"
#include "apps/light_switch/light_switch.h"
#include "apps/music/music.h"
#include "apps/settings/settings.h"
#include "apps/stopwatch/stopwatch.h"

// include all "menu" apps
#include "app_menu.h"
#include "onboarding_menu.h"

// include all "setup" apps
#include "onboarding/hass_setup.h"

// TODO: generate menu based on items in the map

class Apps
{

public:
    Apps();
    void add(app_types type, uint8_t id, App *app);
    void clear();
    EntityStateUpdate update(AppState state);
    TFT_eSprite *renderActive();
    void setActive(app_types type, uint8_t id);
    std::pair<app_types, uint8_t> navigationNext();
    PB_SmartKnobConfig getActiveMotorConfig();
    void setSprite(TFT_eSprite *spr_);
    App *loadApp(uint8_t position, std::string app_slug, std::string app_id, std::string friendly_name);
    void updateMenu();

    void reload(cJSON *apps_);
    void createOnboarding();

private:
    QueueHandle_t mutex;
    std::map<app_types, std::map<uint8_t, std::shared_ptr<App>>> apps;

    uint8_t active_id = 0;

    TFT_eSprite *spr_ = nullptr;
    std::shared_ptr<App> active_app = nullptr;

    TFT_eSprite *rendered_spr_;

    std::shared_ptr<App> find(uint8_t id);
    void lock();
    void unlock();
};
