#if SK_LEDS

#include <FastLED.h>

CRGB leds[NUM_LEDS];
uint8_t ledsBrightness[NUM_LEDS];

uint8_t FULL_BRIGHTNESS = 255;
uint8_t HALF_BRIGHTNESS = 128;

#include "led_ring_task.h"
#include "../semaphore_guard.h"
#include "../util.h"

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

void LedRingTask::renderTrailEffect()
{
    static uint8_t hue = 0; // Static variable to keep track of the hue
    FastLED.setBrightness(255);
    FastLED.clear();
    while (1)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            // Set the current LED to full color with changing hue
            leds[i] = CHSV(hue, 255, 255); // Use CHSV for hue cycling

            // Create the trail effect with dimming colors
            for (int j = 1; j < NUM_LEDS; j++)
            { // Adjusted to potentially cover the whole ring
                int value = 255;
                // Adjust the dimming calculation to accommodate circular logic
                for (int k = 1; k < j; k++)
                {
                    value = floor(value / 30 * 29); // Dimming the brightness for the trail effect
                }
                if (value < 0)
                    value = 0;

                // Calculate the index for the circular array
                int index = (i - j + NUM_LEDS) % NUM_LEDS;

                // Set the color of the trail LEDs with circular wrapping
                leds[index] = CHSV(hue, 255, value);
            }

            FastLED.show();
        }
        hue += 10; // Increment hue after completing the cycle for all LEDs
    }
}

void LedRingTask::renderEffectLightHouse()
{
    const uint32_t frame_ms = 100;
    ledsBrightness[0] = 0;
    if (millis() - effect_statuses[0].last_updated_ms > frame_ms)
    {

        leds[0].setRGB(0, 255, 255);

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
            ledsBrightness[i] = HALF_BRIGHTNESS;
        }

        effect_statuses[1].last_updated_ms = millis();
        FastLED.show();
    }
}

void LedRingTask::renderFadeInEffect()
{

    // Initialize brightness at the lowest level for fade-in effect
    uint8_t brightness = 0;

    while (ledsBrightness[0] < HALF_BRIGHTNESS) // Increase brightness until it reaches full level
    {
        uint32_t colorCode = effect_settings.effect_main_color; // Use the existing color code
        // Extract RGB components from colorCode
        uint8_t r = (colorCode >> 16) & 0xFF;
        uint8_t g = (colorCode >> 8) & 0xFF;
        uint8_t b = colorCode & 0xFF;

        // Scale RGB values by current brightness level to achieve the fade-in effect
        r = (r * brightness) / HALF_BRIGHTNESS;
        g = (g * brightness) / HALF_BRIGHTNESS;
        b = (b * brightness) / HALF_BRIGHTNESS;

        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            // Set the color of each LED in the array with the current brightness level
            leds[i].setRGB(r, g, b);
            ledsBrightness[i] = brightness;
        }

        // Show the LEDs with the updated brightness level
        FastLED.show();

        // Increase brightness for the next frame, until it reaches 255 (full brightness)
        brightness++;
        ESP_LOGI("LED", "Received Fade In Effect request %d", brightness);

        vTaskDelay(pdMS_TO_TICKS(15)); // Wait a bit before increasing the brightness again
    }
}
void LedRingTask::renderFadeOutEffect()
{

    uint8_t brightness = HALF_BRIGHTNESS; // Start with full brightness
    while (ledsBrightness[0] > 0)         // avoid to loop through the effect if you are already at level.
    {
        uint32_t colorCode = effect_settings.effect_main_color; // Use the existing color code
        // Extract RGB components from colorCode
        uint8_t r = (colorCode >> 16) & 0xFF;
        uint8_t g = (colorCode >> 8) & 0xFF;
        uint8_t b = colorCode & 0xFF;

        // Scale RGB values by current brightness level
        r = (r * brightness) / HALF_BRIGHTNESS;
        g = (g * brightness) / HALF_BRIGHTNESS;
        b = (b * brightness) / HALF_BRIGHTNESS;

        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            // Assuming your LEDs array supports setting colors via RGB values directly
            leds[i].setRGB(r, g, b);
            ledsBrightness[i] = brightness;
        }

        // Show the LEDs
        FastLED.show();

        // Decrease brightness for the next frame, stop at 0 to avoid underflow
        if (brightness > 0)
        {
            brightness--;
        }
    }
}

void LedRingTask::run()
{

    FastLED.addLeds<WS2812B, PIN_LED_DATA, GRB>(leds, NUM_LEDS);
    uint8_t hue = 150; // 150 = teal colour.
    for (int i = 0; i < NUM_LEDS; i++)
    {
        // Set the current LED to full color with changing hue
        leds[i] = CHSV(hue, 255, 255); // Use CHSV for hue cycling

        // Create the trail effect with dimming colors
        for (int j = 1; j < NUM_LEDS; j++)
        { // Adjusted to potentially cover the whole ring
            int value = 255;
            // Adjust the dimming calculation to accommodate circular logic
            for (int k = 1; k < j; k++)
            {
                value = floor(value / 30 * 29); // Dimming the brightness for the trail effect
            }
            if (value < 0)
                value = 0;

            // Calculate the index for the circular array
            int index = (i - j + NUM_LEDS) % NUM_LEDS;

            // Set the color of the trail LEDs with circular wrapping
            leds[index] = CHSV(hue, 255, value);
        }

        FastLED.show();
    }
    FastLED.clear();
    FastLED.setBrightness(155);
    FastLED.show();

    while (1)
    {

        // Attempt to receive an item from the render_effect_queue_ without blocking.
        // - render_effect_queue_ is the queue handle from which we're trying to receive an item.
        // - &effect_settings is a pointer to the variable where the received item will be stored.
        // - 0 specifies that the call should not wait (block) if the queue is empty.
        // If an item is successfully received (return value is pdTRUE), the code within the if-statement executes.
        // This is useful for checking the queue and responding to events without delaying program execution.
        if (xQueueReceive(render_effect_queue_, &effect_settings, 0) == pdTRUE)
        {
            // Sync reaction to receiving an effect.
        }

        // At this point, effect_settings is always the last object received through the queue.
        // Todo : turn this on, when verbose.
        // ESP_LOGI("LED", "Received request for effect %d", effect_settings.effect_id);

        switch (effect_settings.effect_id)
        {
        case 0:
            // TODO: disabled for a Demo
            renderEffectSnake();
            break;
        case 1:
            renderEffectStaticColor();
            break;
        case 2:
            renderEffectLightHouse();
            break;
        case 3:
            renderTrailEffect();
            break;
        case 4:
            renderFadeInEffect();
            break;
        case 5:
            renderFadeOutEffect();
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
    xQueueSend(render_effect_queue_, &effect_settings, 0); // portMAX_DELAY
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