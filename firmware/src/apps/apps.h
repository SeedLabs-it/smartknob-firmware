#pragma once

#include <map>

#include "../app_config.h"
#include "../notify/motor_notifier/motor_notifier.h"
#include "../navigation/navigation.h"
#include "./notify/os_config_notifier/os_config_notifier.h"

#include "apps/light_dimmer/light_dimmer.h"
#include "apps/switch/switch.h"
#include "apps/settings/settings.h"
#include "apps/stopwatch/stopwatch.h"
#include "apps/blinds/blinds.h"
#include "apps/climate/climate.h"
#include "apps/spotify/spotify.h"

#include "app_menu.h"

class Apps
{

public:
    Apps(SemaphoreHandle_t mutex);

    void add(uint8_t id, App *app);
    void clear();

    EntityStateUpdate update(AppState state);
    void render();
    void setActive(int8_t id);

    App *loadApp(uint8_t position, std::string app_slug, char *app_id, char *friendly_name, char *entity_id);
    void updateMenu();

    void setMotorNotifier(MotorNotifier *motor_notifier);
    void setOSConfigNotifier(OSConfigNotifier *os_config_notifier);
    void triggerMotorConfigUpdate();
    void handleNavigationEvent(NavigationEvent event);

    PB_SmartKnobConfig blocked_motor_config = {
        .position_width_radians = 60 * M_PI / 180,
        .endstop_strength_unit = 0,
        .snap_point = 0.5,
        .detent_positions_count = 0,
        .detent_positions = {},
    };

protected:
    SemaphoreHandle_t screen_mutex_;
    SemaphoreHandle_t app_mutex_;
    std::map<uint8_t, std::shared_ptr<App>> apps;
    std::shared_ptr<Menu> menu = nullptr;

    int8_t active_id = 0;

    std::shared_ptr<App> active_app = nullptr;

    std::shared_ptr<App> find(uint8_t id);
    std::shared_ptr<App> find(char *app_id);

    PB_SmartKnobConfig root_level_motor_config;

    MotorNotifier *motor_notifier;

    OSConfigNotifier *os_config_notifier_;
};
