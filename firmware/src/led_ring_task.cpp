#if SK_LEDS

#include <FastLED.h>

CRGB leds[NUM_LEDS];

#include "led_ring_task.h"
#include "semaphore_guard.h"
#include "util.h"

LedRingTask::LedRingTask(const uint8_t task_core) : Task{"Led_Ring", 2048 * 2, 1, task_core}
{

    render_effect_queue_ = xQueueCreate(10, sizeof(EffectSettings));

    mutex_ = xSemaphoreCreateMutex();

    assert(mutex_ != NULL);
}

LedRingTask::~LedRingTask()
{
    vQueueDelete(render_effect_queue_);

    vSemaphoreDelete(mutex_);
}

void LedRingTask::renderEffectSnake()
{
    const uint32_t frame_ms = 1000;

    if (millis() - effect_statuses[0].last_updated_ms > frame_ms)
    {
        uint8_t active_led_id = (effect_statuses[0].percent * NUM_LEDS) / 100;

        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {

            if (i == active_led_id)
            {
                leds[i].setRGB(255, 0, 0);
            }
            else
            {
                leds[i].setRGB(10, 0, 0);
            }
        }
        effect_statuses[0].percent++;
        if (effect_statuses[0].percent > 100)
        {
            effect_statuses[0].percent = 0;
        }
        effect_statuses[0].last_updated_ms = millis();
        FastLED.show();
    }
}

void LedRingTask::renderEffectLightHouse()
{
    const uint32_t frame_ms = 100;

    if (millis() - effect_statuses[0].last_updated_ms > frame_ms)
    {

        leds[0].setRGB(255, 255, 255);

        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            if (i == 0)
            {
                continue;
            }
            leds[i].setRGB(0, 0, 0);
        }
        effect_statuses[0].last_updated_ms = millis();
        FastLED.show();
    }
}

void LedRingTask::renderEffectStaticColor()
{
    const uint32_t frame_ms = 1000;

    if (millis() - effect_statuses[1].last_updated_ms > frame_ms)
    {
        // uint8_t active_led_id = (effect_statuses[0].percent * NUM_LEDS) / 100;

        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            leds[i].setColorCode(effect_settings.effect_main_color);
        }

        effect_statuses[1].last_updated_ms = millis();
        FastLED.show();
    }
}

void LedRingTask::run()
{

    FastLED.addLeds<WS2812B, PIN_LED_DATA, GRB>(leds, NUM_LEDS);

    FastLED.setBrightness(255);

    FastLED.clear();

    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].setRGB(255, 255, 255);
        FastLED.show();
        vTaskDelay(pdMS_TO_TICKS(7));
    }

    FastLED.clear();
    FastLED.setBrightness(125);

    while (1)
    {

        if (xQueueReceive(render_effect_queue_, &effect_settings, 0) == pdTRUE)
        {
            // do nothing
        }

        switch (effect_settings.effect_id)
        {
        case 0:
            renderEffectSnake();
            break;
        case 1:
            renderEffectStaticColor();
            break;

        case 2:
            /* code */
            renderEffectLightHouse();
            break;

        default:
            break;
        }

        //

        delay(1);
    }
}

void LedRingTask::setEffect(EffectSettings effect_settings)
{
    // TODO: make it async and safe with a queue
    xQueueSend(render_effect_queue_, &effect_settings, portMAX_DELAY);
}

void LedRingTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void LedRingTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}

#endif