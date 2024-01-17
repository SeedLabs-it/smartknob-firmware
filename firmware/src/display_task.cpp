#if SK_DISPLAY
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "apps/stopwatch.h"
#include "apps/light_switch.h"

#include "cJSON.h"

static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

DisplayTask::DisplayTask(const uint8_t task_core) : Task{"Display", 2048 * 6, 1, task_core}
{
    app_state_queue_ = xQueueCreate(1, sizeof(AppState));
    assert(app_state_queue_ != NULL);

    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
}

DisplayTask::~DisplayTask()
{
    vQueueDelete(app_state_queue_);
    vSemaphoreDelete(mutex_);
}

Apps *DisplayTask::getApps()
{
    return &apps;
}

void DisplayTask::run()
{
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(SK_DISPLAY_ROTATION);
    tft_.fillScreen(TFT_BLACK);

    ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

    log("push menu sprite: ok");

    spr_.setColorDepth(8);

    if (spr_.createSprite(TFT_WIDTH, TFT_HEIGHT) == nullptr)
    {
        log("ERROR: sprite allocation failed!");
        tft_.fillScreen(TFT_RED);
    }
    else
    {
        log("Sprite created!");
        tft_.fillScreen(TFT_BLACK);
    }
    spr_.setTextColor(0xFFFF, TFT_BLACK);

    // std::string apps_config = "[{\"app_slug\":\"stopwatch\",\"app_id\":\"stopwatch.office\",\"friendly_name\":\"Stopwatch\",\"area\":\"office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_switch\",\"app_id\":\"light.ceiling\",\"friendly_name\":\"Ceiling\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_dimmer\",\"app_id\":\"light.workbench\",\"friendly_name\":\"Workbench\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"thermostat\",\"app_id\":\"climate.office\",\"friendly_name\":\"Climate\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"3d_printer\",\"app_id\":\"3d_printer.office\",\"friendly_name\":\"3D Printer\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"blinds\",\"app_id\":\"blinds.office\",\"friendly_name\":\"Shades\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"music\",\"app_id\":\"music.office\",\"friendly_name\":\"Music\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"}]";
    std::string apps_config = "[{\"app_slug\":\"stopwatch\",\"app_id\":\"stopwatch-office\",\"friendly_name\":\"Stopwatch\",\"area\":\"office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_switch\",\"app_id\":\"light.ceiling\",\"friendly_name\":\"Ceiling\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"}]";

    cJSON *json_root = cJSON_Parse(apps_config.c_str());

    if (json_root == NULL)
    {
        ESP_LOGE("display_task.cpp", "failed to parse JSON");
    }

    apps.setSprite(&spr_);

    cJSON *json_app = NULL;

    uint16_t app_position = 1;

    cJSON_ArrayForEach(json_app, json_root)
    {
        cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app, "app_slug");
        cJSON *json_app_id = cJSON_GetObjectItemCaseSensitive(json_app, "app_id");
        cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app, "friendly_name");
        snprintf(buf_, sizeof(buf_), "fromJSON > app_slug=%s", json_app_slug->valuestring);
        log(buf_);
        // ESP_LOGD("display_task.cpp", "%s", buf_);

        apps.loadApp(app_position, std::string(json_app_slug->valuestring), std::string(json_app_id->valuestring), json_friendly_name->valuestring);

        app_position++;
    }

    cJSON_Delete(json_root);

    SettingsApp *settings_app = new SettingsApp(&spr_);
    apps.add(app_position, settings_app);

    // generate menu from apps list
    apps.updateMenu();

    AppState app_state;

    // apps.setActive(3);

    spr_.setTextDatum(CC_DATUM);
    spr_.setTextColor(TFT_WHITE);

    unsigned long last_rendering_ms = millis();
    unsigned long last_fps_check = millis();

    const uint16_t wanted_fps = 60;
    uint16_t fps_counter = 0;

    while (1)
    {

        if (millis() - last_rendering_ms > 1000 / wanted_fps)
        {
            spr_.fillSprite(TFT_BLACK);
            apps.renderActive()->pushSprite(0, 0);

            {
                SemaphoreGuard lock(mutex_);
                ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness_);
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

QueueHandle_t DisplayTask::getKnobStateQueue()
{
    return app_state_queue_;
}

void DisplayTask::setBrightness(uint16_t brightness)
{
    SemaphoreGuard lock(mutex_);
    brightness_ = brightness >> (16 - SK_BACKLIGHT_BIT_DEPTH);
}

void DisplayTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void DisplayTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}

#endif