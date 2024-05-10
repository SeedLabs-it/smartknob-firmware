#if SK_DISPLAY
#include "display_task.h"
#include "semaphore_guard.h"
#include "util.h"

#include "cJSON.h"

static const uint8_t LEDC_CHANNEL_LCD_BACKLIGHT = 0;

#define DISP_BUF_SIZE (TFT_WIDTH * 200) // Larger buffer for LVGL allows for more stable FPS - if memory is a concern buffer size can be reduced at the cost of FPS
uint32_t draw_buf[DISP_BUF_SIZE / 4];

#define TFT_HOR_RES 240
#define TFT_VER_RES 240

DisplayTask::DisplayTask(const uint8_t task_core) : Task{"Display", 1024 * 16, 1, task_core}
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

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    /*Copy `px map` to the `area`*/

    /*For example ("my_..." functions needs to be implemented by you)
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    my_set_window(area->x1, area->y1, w, h);
    my_draw_bitmaps(px_map, w * h);
     */

    /*Call it to tell LVGL you are ready*/
    lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    /*For example  ("my_..." functions needs to be implemented by you)
    int32_t x, y;
    bool touched = my_get_touch( &x, &y );

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;

        data->point.x = x;
        data->point.y = y;
    }
     */
}

void DisplayTask::run()
{
    tft_.begin();
    tft_.invertDisplay(1);
    tft_.setRotation(2);
    tft_.fillScreen(TFT_BLACK);

    ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

    lv_init();
    lv_tick_set_cb((lv_tick_get_cb_t)millis);

#if LV_USE_LOG != 0
    // lv_log_register_print_cb(my_print);
#endif

    lv_display_t *disp;
    disp = lv_skdk_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "LVGL!!!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // onboardingScreen = lv_obj_create(NULL);
    demoScreen = lv_obj_create(NULL);

    lv_obj_t *label_demo = lv_label_create(demoScreen);
    lv_label_set_text(label_demo, "Demo screen");
    lv_obj_align(label_demo, LV_ALIGN_CENTER, 0, 0);

    demo_apps = DemoApps(&spr_);
    hass_apps = HassApps(&spr_);

    onboarding_flow = new OnboardingFlow();

    AppState app_state;

    unsigned long last_rendering_ms = millis();
    unsigned long last_fps_check = millis();

    const uint16_t wanted_fps = 60;
    uint16_t fps_counter = 0;

    // lv_scr_load(onboardingScreen);

    while (1)
    {
        lv_task_handler();
        // if (millis() - last_rendering_ms > 3000)
        // {
        //     LOGE("Free heap: %d", ESP.getFreeHeap());
        //     LOGE("Free stack: %d", uxTaskGetStackHighWaterMark(NULL));
        //     last_rendering_ms = millis();
        // }

        // if (millis() - last_rendering_ms > 1000 / wanted_fps)
        // {

        // spr_.fillSprite(TFT_BLACK);
        // spr_.setTextSize(1);

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
    while (onboarding_flow == nullptr)
    {
        delay(50);
    }
    os_mode = Onboarding;
    onboarding_flow->triggerMotorConfigUpdate();
    // lv_scr_load(onboardingScreen);
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