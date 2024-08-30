#pragma once

#if SK_LEDS

#include "../logger.h"
#include "../task.h"
#include "../app_config.h"

const uint8_t total_effects = 8;

struct EffectStatus
{
    uint8_t percent;
    unsigned long last_updated_ms;
};

enum EffectType
{
    SNAKE = 0,
    STATIC_COLOR = 1,
    LIGHT_HOUSE = 2,
    TRAIL = 3,
    FADE_IN = 4,
    FADE_OUT = 5,
    LEDS_OFF = 6,
    TO_BRIGHTNESS = 7
};

struct EffectSettings
{
    EffectType effect_type;
    uint8_t effect_start_pixel;
    uint8_t effect_end_pixel;
    uint8_t effect_accent_pixel;
    uint32_t effect_main_color;
    uint32_t effect_accent_color;
    uint8_t effect_brightness;
    SETTINGS_LedRing led_ring_settings;
};

class LedRingTask : public Task<LedRingTask>
{
    friend class Task<LedRingTask>; // Allow base Task to invoke protected run()

public:
    LedRingTask(const uint8_t task_core);
    ~LedRingTask();
    void setEffect(EffectSettings effect_settings);

protected:
    void
    run();

private:
    QueueHandle_t render_effect_queue_;

    SemaphoreHandle_t mutex_;

    uint32_t effect_timeout_ms;
    unsigned long effect_expiration_ms;
    EffectSettings effect_settings;

    EffectSettings old_effect_settings;

    EffectStatus effect_statuses[total_effects];

    void renderEffectSnake();
    void renderEffectStaticColor();
    void renderEffectLightHouse();
    void renderTrailEffect();
    void renderFadeInEffect();
    void renderFadeOutEffect();

    void renderToBrightness();
    void ledsOff();
};

#else

class LedRingTask
{
};

#endif
