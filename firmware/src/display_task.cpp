#if SK_DISPLAY
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"
#include "esp_spiram.h"
#include "esp_heap_caps.h"

#include "apps/light_switch/light_switch.h"
#include "apps/light_dimmer/light_dimmer.h"

#include "cJSON.h"

static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

#define TFT_HOR_RES 240
#define TFT_VER_RES 240

#define LVGL_TASK_MAX_DELAY_MS (500)
#define LVGL_TASK_MIN_DELAY_MS (1)

DisplayTask::DisplayTask(const uint8_t task_core) : Task{"Display", 1024 * 24, 2, task_core}
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
    while (onboarding_flow == nullptr)
    {
        delay(50);
    }
    return onboarding_flow;
}

DemoApps *DisplayTask::getDemoApps()
{
    while (demo_apps == nullptr)
    {
        delay(50);
    }
    return demo_apps;
}

HassApps *DisplayTask::getHassApps()
{
    while (hass_apps == nullptr)
    {
        delay(50);
    }
    return hass_apps;
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
    lv_display_t *disp;
    disp = lv_skdk_create();

    onboarding_flow = new OnboardingFlow(mutex_);
    demo_apps = new DemoApps(mutex_);
    hass_apps = new HassApps(mutex_);

    while (display_os_mode == UNSET)
    {
        delay(50);
    }

    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Loading..........!!!!!!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    // lv_scr_load(screen);

    // // uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;

    lv_obj_t *hue_screen = lv_obj_create(screen);
    lv_obj_remove_style_all(hue_screen);
    lv_obj_set_size(hue_screen, LV_HOR_RES, LV_VER_RES);
    // lv_obj_add_flag(hue_screen, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *hue_wheel_img = lv_img_create(hue_screen);
    LV_IMAGE_DECLARE(hue_wheel);
    lv_img_set_src(hue_wheel_img, &hue_wheel);
    lv_obj_set_width(hue_wheel_img, hue_wheel.header.w);
    lv_obj_set_height(hue_wheel_img, hue_wheel.header.h);
    lv_obj_align(hue_wheel_img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *mask_img = lv_img_create(hue_screen);
    LV_IMAGE_DECLARE(a8_transp_mask);
    lv_img_set_src(mask_img, &a8_transp_mask);
    lv_obj_set_width(mask_img, a8_transp_mask.header.w);
    lv_obj_set_height(mask_img, a8_transp_mask.header.h);
    lv_obj_align(mask_img, LV_ALIGN_CENTER, 0, 0);

    lv_scr_load(screen);

    while (1)
    {
        // {
        //     SemaphoreGuard lock(mutex_);
        //     lv_image_set_rotation(mask_img, lv_image_get_rotation(mask_img) + 10);
        // }
        vTaskDelay(pdMS_TO_TICKS(2));
        {
            SemaphoreGuard lock(mutex_);
            lv_task_handler();

            // task_delay_ms = lv_timer_handler();
        }
        // if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS)
        // {
        //     task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
        // }
        // else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS)
        // {
        //     task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
        // }
        // vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
        // delay(5);
    }
}

QueueHandle_t DisplayTask::getKnobStateQueue()
{
    return app_state_queue_;
}

void DisplayTask::setBrightness(uint16_t brightness)
{
    SemaphoreGuard lock(mutex_);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness >> (16 - SK_BACKLIGHT_BIT_DEPTH));
}

void DisplayTask::enableOnboarding()
{
    display_os_mode = ONBOARDING;
    onboarding_flow->render();
    onboarding_flow->triggerMotorConfigUpdate();
}

void DisplayTask::enableDemo()
{
    display_os_mode = DEMO;
    demo_apps->render();
    demo_apps->triggerMotorConfigUpdate();
    // lv_scr_load(demoScreen);
}

void DisplayTask::enableHass()
{
    display_os_mode = HASS;
    // hass_apps.triggerMotorConfigUpdate();
}
#endif