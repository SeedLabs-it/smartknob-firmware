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
    delay(1000);
    ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

    lv_init();
    lv_disp_drv_t *disp_drv = lv_skdk_create();

    onboarding_flow = new OnboardingFlow(mutex_);
    demo_apps = new DemoApps(mutex_);
    hass_apps = new HassApps(mutex_);

    while (display_os_mode == UNSET)
    {
        delay(50);
    }

    while (1)
    {
        {
            SemaphoreGuard lock(mutex_);
            lv_task_handler();
        }
        vTaskDelay(pdMS_TO_TICKS(5));
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