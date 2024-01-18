#include "demo.h"

#include "../settings.h"
#include "semaphore_guard.h"

DemoApp::DemoApp(TFT_eSprite *spr_) : App(spr_)
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0, // max position < min position indicates no bounds
        60 * PI / 180,
        1,
        1,
        0.55,
        "SKDEMO_Demo", // TODO: clean this
        0,
        {},
        0,
        20,
    };
}

EntityStateUpdate DemoApp::updateStateFromKnob(PB_SmartKnobState state)
{
}

void DemoApp::updateStateFromSystem(AppState state)
{
}

uint8_t DemoApp::navigationNext()
{
    return 0; // +1 to shift from 0 position which is menu itself
}

TFT_eSprite *DemoApp::render()
{
    std::string apps_config = "[{\"app_slug\":\"stopwatch\",\"app_id\":\"stopwatch.office\",\"friendly_name\":\"Stopwatch\",\"area\":\"office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_switch\",\"app_id\":\"light.ceiling\",\"friendly_name\":\"Ceiling\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_dimmer\",\"app_id\":\"light.workbench\",\"friendly_name\":\"Workbench\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"thermostat\",\"app_id\":\"climate.office\",\"friendly_name\":\"Climate\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"3d_printer\",\"app_id\":\"3d_printer.office\",\"friendly_name\":\"3D Printer\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"blinds\",\"app_id\":\"blinds.office\",\"friendly_name\":\"Shades\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"music\",\"app_id\":\"music.office\",\"friendly_name\":\"Music\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"}]";

    cJSON *json_root = cJSON_Parse(apps_config.c_str());

    if (json_root == NULL)
    {
        ESP_LOGE("display_task.cpp", "failed to parse JSON");
    }

    apps->setSprite(spr_);

    cJSON *json_app = NULL;

    uint16_t app_position = 1;

    cJSON_ArrayForEach(json_app, json_root)
    {
        cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app, "app_slug");
        cJSON *json_app_id = cJSON_GetObjectItemCaseSensitive(json_app, "app_id");
        cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app, "friendly_name");

        apps->loadApp(app_position, std::string(json_app_slug->valuestring), std::string(json_app_id->valuestring), json_friendly_name->valuestring);

        app_position++;
    }

    cJSON_Delete(json_root);

    SettingsApp *settings_app = new SettingsApp(spr_);
    apps->add(app_position, settings_app);

    apps->updateMenu();

    unsigned long last_rendering_ms = millis();
    unsigned long last_fps_check = millis();

    const uint16_t wanted_fps = 60;
    uint16_t fps_counter = 0;

    while (1)
    {

        if (millis() - last_rendering_ms > 1000 / wanted_fps)
        {
            spr_->fillSprite(TFT_BLACK);
            apps->renderActive()->pushSprite(0, 0);

            {
                SemaphoreGuard lock(mutex_);
                // ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness_);
            }
            last_rendering_ms = millis();

            fps_counter++;
            if (last_fps_check + 1000 < millis())
            {
                ESP_LOGD("display_task.cpp", "Screen real FPS %d", fps_counter);
                fps_counter = 0;
                last_fps_check = millis();
            }
        }

        delay(1);
    }
}