#if SK_LEDS

#include <FastLED.h>

CRGB leds[NUM_LEDS];
uint8_t ledsBrightness[NUM_LEDS];

uint8_t FULL_BRIGHTNESS = 125;

#include "led_ring_task.h"
#include "../semaphore_guard.h"
#include "../util.h"

LedRingTask::LedRingTask(const uint8_t task_core) : Task{"Led_Ring", 1024 * 4, 1, task_core}
{
    mutex_ = xSemaphoreCreateMutex();

    assert(mutex_ != NULL);
}

LedRingTask::~LedRingTask()
{
    vQueueDelete(render_effect_queue_);

    vSemaphoreDelete(mutex_);
}

// void LedRingTask::renderEffectSnake()
// {
//     const uint32_t frame_ms = 1000;

//     if (millis() - effect_statuses[0].last_updated_ms > frame_ms)
//     {
//         uint8_t active_led_id = (effect_statuses[0].percent * NUM_LEDS) / 100;

//         for (uint8_t i = 0; i < NUM_LEDS; i++)
//         {

//             if (i == active_led_id)
//             {
//                 leds[i].setRGB(255, 0, 0);
//             }
//             else
//             {
//                 leds[i].setRGB(10, 0, 0);
//             }
//         }
//         effect_statuses[0].percent++;
//         if (effect_statuses[0].percent > 100)
//         {
//             effect_statuses[0].percent = 0;
//         }
//         effect_statuses[0].last_updated_ms = millis();
//         FastLED.show();
//     }
// }

// void LedRingTask::renderTrailEffect()
// {
//     static uint8_t hue = 0; // Static variable to keep track of the hue
//     FastLED.setBrightness(255);
//     FastLED.clear();
//     while (1)
//     {
//         for (int i = 0; i < NUM_LEDS; i++)
//         {
//             // Set the current LED to full color with changing hue
//             leds[i] = CHSV(hue, 255, 255); // Use CHSV for hue cycling

//             // Create the trail effect with dimming colors
//             for (int j = 1; j < NUM_LEDS; j++)
//             { // Adjusted to potentially cover the whole ring
//                 int value = 255;
//                 // Adjust the dimming calculation to accommodate circular logic
//                 for (int k = 1; k < j; k++)
//                 {
//                     value = floor(value / 30 * 29); // Dimming the brightness for the trail effect
//                 }
//                 if (value < 0)
//                     value = 0;

//                 // Calculate the index for the circular array
//                 int index = (i - j + NUM_LEDS) % NUM_LEDS;

//                 // Set the color of the trail LEDs with circular wrapping
//                 leds[index] = CHSV(hue, 255, value);
//             }

//             FastLED.show();
//         }
//         hue += 10; // Increment hue after completing the cycle for all LEDs
//     }
// }

// void LedRingTask::renderEffectLightHouse()
// {
//     const uint32_t frame_ms = 100;

//     bool exitCriteriaMet = false;
//     while (exitCriteriaMet == false)
//     {
//         exitCriteriaMet = true;
//         for (uint8_t i = 0; i < NUM_LEDS; i++)
//         {
//             if (i == 0)
//             {
//                 if (ledsBrightness[0] < FULL_BRIGHTNESS)
//                 {
//                     leds[0].setRGB(0, ledsBrightness[0], ledsBrightness[0]);
//                     ledsBrightness[0]++;
//                     exitCriteriaMet = false;
//                 }
//             }
//             else
//             {
//                 if (ledsBrightness[i] > 0)
//                 {

//                     ledsBrightness[i]--;
//                     leds[i].setRGB(0, ledsBrightness[i], ledsBrightness[i]);
//                     exitCriteriaMet = false;
//                 }
//             }
//         }

//         FastLED.show();
//     }
// }

void LedRingTask::renderEffectStaticColor(LedRingEffect effect)
{
    const uint32_t frame_ms = 50;

    if (millis() - current_effect_last_updated_ms > frame_ms)
    {
        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            leds[i].setColorCode(effect.payload.static_color.color);
            // TODO: add validation for max brightness
            ledsBrightness[i] = effect.payload.static_color.brightness > FULL_BRIGHTNESS ? FULL_BRIGHTNESS : effect.payload.static_color.brightness;
        }

        current_effect_last_updated_ms = millis();
        FastLED.show();
    }
}

void LedRingTask::renderEffectStaticColorRange(LedRingEffect effect)
{
    const uint32_t frame_ms = 50;

    EffectStaticColorRange payload = effect.payload.static_color_range;

    if (millis() - current_effect_last_updated_ms > frame_ms)
    {
        int16_t current_led_degree = 0;

        if (payload.from_degrees < 0)
        {
            payload.from_degrees = 360 + payload.from_degrees;
        }

        if (payload.to_degrees < 0)
        {
            payload.to_degrees = 360 + payload.to_degrees;
        }

        for (int16_t i = NUM_LEDS - 1; i >= 0; i--)
        {

            if (i > NUM_LEDS - (NUM_LEDS / 4))
            {
                // 360 - 270 deg
                current_led_degree = 360 - 360 / 4 + ((NUM_LEDS - i)) * 5;
            }
            else
            {
                current_led_degree = (NUM_LEDS - i - (NUM_LEDS / 4)) * 5;
            }

            // if (payload.from_degrees > payload.to_degrees)
            // {
            //     if (current_led_degree > payload.from_degrees ||)
            //     {

            //         leds[i].setColorCode(payload.color);
            //         // TODO: add validation for max brightness
            //         ledsBrightness[i] = payload.brightness;
            //     }
            //     else
            //     {
            //         leds[i].setColorCode(0);
            //     }
            // }

            // if (current_led_degree > payload.from_degrees && current_led_degree < payload.to_degrees)
            // {

            //     leds[i].setColorCode(payload.color);
            //     // TODO: add validation for max brightness
            //     ledsBrightness[i] = payload.brightness;
            // }
            // else
            // {
            //     leds[i].setColorCode(0);
            // }

            if (payload.from_degrees > payload.to_degrees)
            {
                if (current_led_degree >= payload.from_degrees || current_led_degree <= payload.to_degrees)
                {

                    leds[i].setColorCode(payload.color);
                    // TODO: add validation for max brightness
                    ledsBrightness[i] = payload.brightness > FULL_BRIGHTNESS ? FULL_BRIGHTNESS : payload.brightness;
                }
                else
                {
                    leds[i].setColorCode(0);
                }
            }
            else
            {
                if (current_led_degree >= payload.from_degrees && current_led_degree <= payload.to_degrees)
                {

                    leds[i].setColorCode(payload.color);
                    // TODO: add validation for max brightness
                    ledsBrightness[i] = payload.brightness > FULL_BRIGHTNESS ? FULL_BRIGHTNESS : payload.brightness;
                }
                else
                {
                    leds[i].setColorCode(0);
                }
            }
        }

        current_effect_last_updated_ms = millis();
        FastLED.show();
    }
}

// void LedRingTask::renderFadeInEffect()
// {
//     uint32_t colorCode = effect_settings.effect_main_color; // Use the existing color code
//     // Extract RGB components from colorCode
//     uint8_t r = (colorCode >> 16) & 0xFF;
//     uint8_t g = (colorCode >> 8) & 0xFF;
//     uint8_t b = colorCode & 0xFF;

//     bool exitCriteriaMet = false;    // this is true when all the leds are at full brightness.
//     while (exitCriteriaMet == false) // Increase brightness until it reaches full level
//     {
//         exitCriteriaMet = true;
//         // Scale RGB values by current brightness level to achieve the fade-in effect
//         for (uint8_t i = 0; i < NUM_LEDS; i++)
//         {
//             // Set the color of each LED in the array with the current brightness level
//             if (ledsBrightness[i] < FULL_BRIGHTNESS)
//             {
//                 exitCriteriaMet = false;
//                 leds[i].setRGB((r * ledsBrightness[i]) / FULL_BRIGHTNESS,
//                                (g * ledsBrightness[i]) / FULL_BRIGHTNESS,
//                                (b * ledsBrightness[i]) / FULL_BRIGHTNESS);
//                 ledsBrightness[i]++;
//             }
//         }

//         // Show the LEDs with the updated brightness level
//         FastLED.show();

//         // Increase brightness for the next frame, until it reaches 255 (full brightness)
//     }
// }
// void LedRingTask::renderFadeOutEffect()
// {

//     uint32_t colorCode = effect_settings.effect_main_color; // Use the existing color code
//     // Extract RGB components from colorCode
//     uint8_t r = (colorCode >> 16) & 0xFF;
//     uint8_t g = (colorCode >> 8) & 0xFF;
//     uint8_t b = colorCode & 0xFF;

//     bool exitCriteriaMet = false;    // This becomes true when all the LEDs are at minimum brightness.
//     while (exitCriteriaMet == false) // Decrease brightness until it reaches zero level
//     {
//         exitCriteriaMet = true;
//         // Scale RGB values by current brightness level to achieve the fade-out effect
//         for (uint8_t i = 0; i < NUM_LEDS; i++)
//         {
//             // Decrease the color brightness of each LED in the array towards zero
//             if (ledsBrightness[i] > 0)
//             {
//                 exitCriteriaMet = false;
//                 ledsBrightness[i]--;
//                 leds[i].setRGB((r * ledsBrightness[i]) / FULL_BRIGHTNESS,
//                                (g * ledsBrightness[i]) / FULL_BRIGHTNESS,
//                                (b * ledsBrightness[i]) / FULL_BRIGHTNESS);
//             }
//         }

//         // Show the LEDs with the updated brightness level
//         FastLED.show();

//         // The loop continues until all LEDs have been dimmed to zero brightness
//     }
// }

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
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    FastLED.clear();
    FastLED.setBrightness(FULL_BRIGHTNESS);
    FastLED.show();

    LedRingEffect tmp_effect;

    unsigned long now = millis();

    while (1)
    {

        // Attempt to receive an item from the render_effect_queue_ without blocking.
        // - render_effect_queue_ is the queue handle from which we're trying to receive an item.
        // - &effect_settings is a pointer to the variable where the received item will be stored.
        // - 0 specifies that the call should not wait (block) if the queue is empty.
        // If an item is successfully received (return value is pdTRUE), the code within the if-statement executes.
        // This is useful for checking the queue and responding to events without delaying program execution.
        if (xQueueReceive(led_ring_notifier_.getQueue(), &tmp_effect, 0) == pdTRUE)
        {
            // Sync reaction to receiving an effect.
            switch (tmp_effect.priprity)
            {
            case LED_RING_PRIOTITY_IDLE:
                current_effect_idle_ = tmp_effect;
                break;
            case LED_RING_PRIOTITY_APP:
                current_effect_app_ = tmp_effect;
                break;

            case LED_RING_PRIOTITY_ERROR:
                current_effect_error_ = tmp_effect;
                break;

            default:
                break;
            }
        }

        // At this point, effect_settings is always the last object received through the queue.
        // Todo : turn this on, when verbose.
        // ESP_LOGI("LED", "Received request for effect %d", effect_settings.effect_id);

        now = millis();

        if (current_effect_error_.expire_at > now)
        {
            current_effect_ = current_effect_error_;
        }
        else if (current_effect_app_.expire_at > now)
        {
            current_effect_ = current_effect_app_;
        }
        else if (current_effect_idle_.expire_at > now)
        {
            current_effect_ = current_effect_idle_;
        }

        switch (current_effect_.type)
        {
        case 0:
            // TODO: disabled for a Demo
            // renderEffectSnake();
            break;
        case STATIC_COLOR:
            renderEffectStaticColor(current_effect_);
            break;
        case STATIC_COLOR_RANGE:
            renderEffectStaticColorRange(current_effect_);
            break;
            // case 2:
            //     renderEffectLightHouse();
            //     break;
            // case 3:
            //     renderTrailEffect();
            //     break;
            // case 4:
            //     renderFadeInEffect();
            //     break;
            // case 5:
            //     renderFadeOutEffect();
            //     break;

        default:
            break;
        }

        //

        delay(1);
    }
}

void LedRingTask::setEffect(LedRingEffect led_ring_effect)
{
    // TODO: make it async and safe with a queue
    xQueueSend(render_effect_queue_, &led_ring_effect, 0); // portMAX_DELAY
}

void LedRingTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

LedRingNotifier *LedRingTask::getNotifier()
{
    return &led_ring_notifier_;
}

void LedRingTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}

#endif