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

// /* Define the points of the line */
// static lv_point_t line_points[] = {{10, 30}, {50, 60}};

// /* Function to move the line */
// void move_line(lv_obj_t *line, lv_point_t *points, int num_points, int dx, int dy)
// {
//     for (int i = 0; i < num_points; i++)
//     {
//         points[i].x += dx * 2;
//         points[i].y += dy * 2;
//     }
//     lv_line_set_points(line, points, num_points); /* Update the points */
// }

// static int current = 0;

// /* Task to move the line around */
// void move_line_task(lv_timer_t *timer)
// {
//     static int dx = 1;
//     static int dy = 1;
//     static lv_obj_t *line1 = (lv_obj_t *)timer->user_data;

//     lv_color_t color = lv_color_hsv_to_rgb(current % 360, 100, 100);
//     // lv_obj_set_style_bg_color(line1, color, LV_PART_MAIN);
//     lv_obj_set_style_line_color(line1, color, LV_PART_MAIN);
//     move_line(line1, line_points, 2, dx, dy);

//     current += 5;

//     /* Change direction if the line reaches the screen edge */
//     for (int i = 0; i < 2; i++)
//     {
//         if (line_points[i].x < 0 || line_points[i].x > lv_disp_get_hor_res(NULL) ||
//             line_points[i].y < 0 || line_points[i].y > lv_disp_get_ver_res(NULL))
//         {
//             dx = -dx;
//             dy = -dy;
//             break;
//         }
//     }
// }

// /* Create the line and start the task */
// void create_line_and_move(void)
// {
//     /* Create a line style */
//     static lv_style_t style_line;
//     lv_style_init(&style_line);
//     lv_style_set_line_width(&style_line, 4);
//     // lv_style_set_line_color(&style_line, lv_color_black());
//     lv_style_set_line_rounded(&style_line, true);

//     /* Create a line object */
//     lv_obj_t *line1 = lv_line_create(lv_scr_act());
//     lv_line_set_points(line1, line_points, 2); /* Set the points */
//     lv_obj_add_style(line1, &style_line, 0);   /* Add the style */
//     // lv_obj_center(line1);                      /* Center the line */

//     /* Create a task to move the line */
//     // lv_task_create(move_line_task, 50, LV_TASK_PRIO_LOW, line1);
//     lv_timer_create(move_line_task, 1, line1);
// }

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

    // lv_obj_t *screen = lv_obj_create(NULL);

    while (display_os_mode == UNSET)
    {
        delay(50);
    }

    delay(1000);
    // lv_scr_load(screen);

    // create_line_and_move();

    while (1)
    {
        {
            // SemaphoreGuard lock(mutex_);
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