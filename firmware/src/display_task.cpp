#if SK_DISPLAY
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "apps/stopwatch.h"
#include "apps/light_switch.h"

#include "font/NDS1210pt7b.h"

#include "cJSON.h"

static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

DisplayTask::DisplayTask(const uint8_t task_core) : Task{"Display", 2048 * 6, 1, task_core}
{
    app_state_queue_ = xQueueCreate(1, sizeof(AppState));
    assert(app_state_queue_ != NULL);

    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(SK_DISPLAY_ROTATION);
    tft_.fillScreen(TFT_BLACK);

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

    spr_.setTextDatum(CC_DATUM);
    spr_.setTextColor(TFT_WHITE);

    apps.setSprite(&spr_);

    onboarding = Onboarding(&spr_);
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

Onboarding *DisplayTask::getOnboarding()
{
    return &onboarding;
}

void DisplayTask::run()
{

    ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

    log("push menu sprite: ok");

    unsigned long last_rendering_ms = millis();
    unsigned long last_fps_check = millis();

    const uint16_t wanted_fps = 60;
    uint16_t fps_counter = 0;

    while (1)
    {

        if (millis() - last_rendering_ms > 1000 / wanted_fps)
        {
            spr_.fillSprite(TFT_BLACK);
            if (false)
                apps.renderActive()->pushSprite(0, 0);
            else
                onboarding.renderActive()->pushSprite(0, 0);

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