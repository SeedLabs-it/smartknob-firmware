#include "onboarding.h"

Onboarding::Onboarding(TFT_eSprite *spr_) : Apps()
{

    this->spr_ = spr_;
    clear();
    OnboardingMenu *onboarding_menu = new OnboardingMenu(this->spr_);

    onboarding_menu->add_item(
        0,
        MenuItem{
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

    onboarding_menu->add_item(
        1,
        MenuItem{
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

    onboarding_menu->add_item(
        2,
        MenuItem{
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
    onboarding_menu->add_item(
        3,
        MenuItem{
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
    onboarding_menu->add_item(
        4,
        MenuItem{
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

    add(menu_type, 0, onboarding_menu);

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
        MenuItemOld{
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
            MenuItemOld{
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
