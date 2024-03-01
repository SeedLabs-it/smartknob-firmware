#if SK_DISPLAY
#include <Arduino.h>
#include "task.h"
#include "semphr.h"
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "display_buffer.h"
#include "cJSON.h"

static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

DisplayTask::DisplayTask(const uint8_t task_core) : Task{"Display", 1024 * 12, 1, task_core}
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

OnboardingFlow *DisplayTask::getOnboardingFlow()
{
    return &onboarding_flow;
}

HassApps *DisplayTask::getHassApps()
{
    return &hass_apps;
}

void DisplayTask::anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)var, v);
}

void DisplayTask::anim_size_cb(void *var, int32_t v)
{
    lv_obj_set_size((lv_obj_t *)var, v, v);
}


void DisplayTask::lvglrun()
{

    lv_obj_t *label;

    lv_obj_clean(lv_screen_active());
    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_font(label, lv_font_default(), 0);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(obj, lv_color_make(0xff, 0x00, 0x00), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 10, 50);
    lv_anim_set_duration(&a, 3000);
    lv_anim_set_playback_delay(&a, 300);
    lv_anim_set_playback_duration(&a, 900);
    lv_anim_set_repeat_delay(&a, 1500);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a, DisplayTask::anim_size_cb);
    lv_anim_start(&a);
    lv_anim_set_exec_cb(&a, DisplayTask::anim_x_cb);
    lv_anim_set_values(&a, 10, 150);
    lv_anim_start(&a);
    while (1)
    {
        // DisplayBuffer::getInstance()->startDrawTransaction();
        smartknob_lvgl_next(); /* let the GUI do its work */
        vTaskDelay(20 / portTICK_PERIOD_MS);
        // DisplayBuffer::getInstance()->endDrawTransaction();
    }

    while (1)
    {
        displayBuffer->startDrawTransaction();
        displayBuffer->getTftBuffer()->fillSprite(TFT_BLACK);
        displayBuffer->endDrawTransaction();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        displayBuffer->startDrawTransaction();
        displayBuffer->getTftBuffer()->fillSprite(TFT_RED);
        displayBuffer->endDrawTransaction();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        displayBuffer->startDrawTransaction();
        displayBuffer->getTftBuffer()->fillSprite(TFT_GREEN);
        displayBuffer->endDrawTransaction();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        displayBuffer->startDrawTransaction();
        displayBuffer->getTftBuffer()->fillSprite(TFT_WHITE);
        displayBuffer->endDrawTransaction();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        displayBuffer->startDrawTransaction();
        displayBuffer->getTftBuffer()->fillSprite(TFT_LIGHTGREY);
        displayBuffer->endDrawTransaction();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        displayBuffer->startDrawTransaction();
        displayBuffer->getTftBuffer()->fillSprite(TFT_ORANGE);
        displayBuffer->endDrawTransaction();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void DisplayTask::run()
{
#ifdef USE_DISPLAY_BUFFER
    displayBuffer = DisplayBuffer::getInstance();
#else
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(SK_DISPLAY_ROTATION);
    tft_.fillScreen(TFT_BLACK);
#endif

    ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

    log("push menu sprite: ok");
    lvglrun();

#ifdef USE_DISPLAY_BUFFER
    hass_apps = HassApps();
    onboarding_flow = OnboardingFlow();
#else
    spr_.setColorDepth(16);

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
    hass_apps = HassApps(&spr_);
    onboarding_flow = OnboardingFlow(&spr_);
#endif

    AppState app_state;
#ifdef USE_DISPLAY_BUFFER
    displayBuffer->getTftBuffer()->setTextDatum(CC_DATUM);
    displayBuffer->getTftBuffer()->setTextColor(TFT_WHITE);
#else
    spr_.setTextDatum(CC_DATUM);
    spr_.setTextColor(TFT_WHITE);
#endif

    unsigned long last_rendering_ms = millis();
    unsigned long last_fps_check = millis();

    const uint16_t wanted_fps = 10;
    uint16_t fps_counter = 0;

    while (1)
    {

        if (millis() - last_rendering_ms > 1000 / wanted_fps)
        {
#ifdef USE_DISPLAY_BUFFER
            displayBuffer->startDrawTransaction();
            displayBuffer->getTftBuffer()->fillSprite(TFT_BLACK);
            displayBuffer->getTftBuffer()->setTextSize(1);
#else
            spr_.fillSprite(TFT_BLACK);
            spr_.setTextSize(1);
#endif

            switch (os_mode)
            {
            case Onboarding:
#ifdef USE_DISPLAY_BUFFER
                onboarding_flow.render();
#else
                onboarding_flow.render()->pushSprite(0, 0);
#endif
                break;
            case Demo:
#ifdef USE_DISPLAY_BUFFER
                displayBuffer->getTftBuffer()->setTextDatum(CC_DATUM);
                displayBuffer->getTftBuffer()->setFreeFont(&NDS1210pt7b);
                displayBuffer->getTftBuffer()->setTextColor(TFT_WHITE);
                displayBuffer->getTftBuffer()->drawString("DEMO", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
#else
                spr_.setTextDatum(CC_DATUM);
                spr_.setFreeFont(&NDS1210pt7b);
                spr_.setTextColor(TFT_WHITE);
                spr_.drawString("DEMO", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
                spr_.pushSprite(0, 0);
#endif
                break;
            case Hass:
#ifdef USE_DISPLAY_BUFFER
                hass_apps.render();
#else
                hass_apps.renderActive()->pushSprite(0, 0);
#endif
                break;
            default:
                break;
            }
            displayBuffer->endDrawTransaction();

            {
                SemaphoreGuard lock(mutex_);
                ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness_);
            }
            last_rendering_ms = millis();

            fps_counter++;
            if (last_fps_check + 1000 < millis())
            {
                // ESP_LOGD("display_task.cpp", "Screen real FPS %d", fps_counter);
                fps_counter = 0;
                last_fps_check = millis();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
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

void DisplayTask::enableOnboarding()
{
    os_mode = Onboarding;
    onboarding_flow.triggerMotorConfigUpdate();
}

void DisplayTask::enableHass()
{
    os_mode = Hass;
}

void DisplayTask::enableDemo()
{
    os_mode = Demo;
}


#endif