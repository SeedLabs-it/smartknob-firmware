#if SK_DISPLAY
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "esp_spiram.h"
#include "esp_heap_caps.h"

#include "cJSON.h"

static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

#define TFT_HOR_RES 240
#define TFT_VER_RES 240

static const uint32_t DISP_BUF_SIZE = ((TFT_HOR_RES * TFT_HOR_RES) * sizeof(lv_color_t));

static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

DisplayTask::DisplayTask(const uint8_t task_core) : Task{"Display", 1024 * 24, 1, task_core}
{
    app_state_queue_ = xQueueCreate(1, sizeof(AppState));
    assert(app_state_queue_ != NULL);

    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    buf1 = (lv_color_t *)heap_caps_aligned_alloc(16, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf1 != NULL);

    buf2 = (lv_color_t *)heap_caps_aligned_alloc(16, DISP_BUF_SIZE, MALLOC_CAP_SPIRAM);
    assert(buf2 != NULL);
}

DisplayTask::~DisplayTask()
{
    heap_caps_free(buf1);
    heap_caps_free(buf2);
    vQueueDelete(app_state_queue_);
    vSemaphoreDelete(mutex_);
}

OnboardingFlow *DisplayTask::getOnboardingFlow()
{
    while (onboarding_flow == nullptr)
    {
        delay(50);
    }
    return onboarding_flow;
}

DemoApps *DisplayTask::getDemoApps()
{
    return &demo_apps;
}

HassApps *DisplayTask::getHassApps()
{
    return &hass_apps;
}

ErrorHandlingFlow *DisplayTask::getErrorHandlingFlow()
{
    return &error_handling_flow;
}

void DisplayTask::run()
{
    ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

    lv_init();
    lv_tick_set_cb((lv_tick_get_cb_t)millis);

    lv_display_t *disp;
    disp = lv_skdk_create(TFT_HOR_RES, TFT_VER_RES, buf1, buf2, DISP_BUF_SIZE);

    demoScreen = lv_obj_create(NULL);
    lv_obj_t *label_demo = lv_label_create(demoScreen);
    lv_label_set_text(label_demo, "Demo screen");
    lv_obj_align(label_demo, LV_ALIGN_CENTER, 0, 0);

    demo_apps = DemoApps(&spr_);
    hass_apps = HassApps(&spr_);

    onboarding_flow = new OnboardingFlow(mutex_);

    AppState app_state;

    unsigned long last_rendering_ms = millis();
    unsigned long last_fps_check = millis();

    const uint16_t wanted_fps = 60;
    uint16_t fps_counter = 0;

    lv_obj_t *rect = lv_obj_create(lv_scr_act());  // Create a new object
    lv_obj_set_size(rect, LV_HOR_RES, LV_VER_RES); // Set the size to cover the entire screen
    lv_obj_set_pos(rect, 0, 0);                    // Position it at the top-left corner
    lv_color_t colors[] = {LV_COLOR_MAKE(255, 0, 255), LV_COLOR_MAKE(255, 0, 0), LV_COLOR_MAKE(0, 255, 0), LV_COLOR_MAKE(0, 0, 255)};

    while (1)
    {

        // SemaphoreGuard lock(mutex_);

        // for (int i = 0; i < 100000; i++)
        // {
        //     lv_obj_set_style_bg_color(rect, colors[i % 4], 0);
        //     lv_task_handler();
        //     delay(200);
        // }

        // if (error_handling_flow.getErrorType() == NO_ERROR)
        // {
        //     switch (os_mode)
        //     {
        //     case Onboarding:
        //         onboarding_flow.render()->pushSprite(0, 0);
        //         break;
        //     case Demo:
        //         demo_apps.renderActive()->pushSprite(0, 0);
        //         break;
        //     case Hass:
        //         hass_apps.renderActive()->pushSprite(0, 0);
        //         break;
        //     default:
        //         spr_.pushSprite(0, 0);
        //         break;
        //     }
        // }
        // else
        // {
        //     error_handling_flow.render()->pushSprite(0, 0);
        // }

        // {
        //     SemaphoreGuard lock(mutex_);
        //     ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness_);
        // }
        // last_rendering_ms = millis();

        // fps_counter++;
        // if (last_fps_check + 1000 < millis())
        // {
        //     fps_counter = 0;
        //     last_fps_check = millis();
        // }
        // }

        // vTaskDelay(pdMS_TO_TICKS(5));
        {
            SemaphoreGuard lock(mutex_);
            lv_task_handler();
            ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness_);
        }
        delay(5);
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

void DisplayTask::enableOnboarding()
{
    os_mode = Onboarding;
    onboarding_flow->render();
    onboarding_flow->triggerMotorConfigUpdate();
}

void DisplayTask::enableDemo()
{
    os_mode = Demo;
    demo_apps.triggerMotorConfigUpdate();
    lv_scr_load(demoScreen);
}

void DisplayTask::enableHass()
{
    os_mode = Hass;
    hass_apps.triggerMotorConfigUpdate();
}
#endif