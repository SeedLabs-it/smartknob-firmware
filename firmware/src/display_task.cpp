#if SK_DISPLAY
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

void DisplayTask::run()
{
    
    spr_ = DisplayBuffer::getInstance()->getTftBuffer();
    ledcSetup(LEDC_CHANNEL_LCD_BACKLIGHT, 5000, SK_BACKLIGHT_BIT_DEPTH);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCD_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, (1 << SK_BACKLIGHT_BIT_DEPTH) - 1);

    log("push menu sprite: ok");

    hass_apps = HassApps(spr_);

    onboarding_flow = OnboardingFlow(spr_);

    AppState app_state;

    spr_->setTextColor(0xFFFF, TFT_BLACK);
    spr_->setTextDatum(CC_DATUM);
    spr_->setTextColor(TFT_WHITE);
    

    while (1)
    {
        DisplayBuffer::getInstance()->startDrawTransaction();
        spr_->fillSprite(TFT_BLACK);
        spr_->setTextSize(1);

        switch (os_mode)
        {
        case Onboarding:
            onboarding_flow.render();
            break;
        case Demo:
            // spr_.setTextDatum(CC_DATUM);
            // spr_.setFreeFont(&NDS1210pt7b);
            // spr_.setTextColor(TFT_WHITE);
            // spr_.drawString("DEMO", TFT_WIDTH / 2, TFT_HEIGHT / 2, 1);
            // spr_.pushSprite(0, 0);
            // break;
        case Hass:
            hass_apps.renderActive();
            break;
        default:
            break;
        }
        DisplayBuffer::getInstance()->endDrawTransaction();

        {
            SemaphoreGuard lock(mutex_);
            ledcWrite(LEDC_CHANNEL_LCD_BACKLIGHT, brightness_);
        }

        vTaskDelay(pdMS_TO_TICKS(25));
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
    hass_apps.triggerMotorConfigUpdate();
}

void DisplayTask::enableDemo()
{
    os_mode = Demo;
}

#endif