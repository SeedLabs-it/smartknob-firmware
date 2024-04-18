#pragma once

#if SK_LEDS

#include "../logger.h"
#include "../task.h"
#include "../app_config.h"
#include "led_ring_notifier.h"

const uint8_t total_effects = 1;

struct EffectStaticColor
{
    uint32_t color;
    uint8_t brightness;
};

struct EffectStaticColorRange
{
    uint32_t color;
    uint8_t brightness;
    int16_t from_degrees;
    int16_t to_degrees;
};

enum LedRingEffectType
{
    STATIC_COLOR = 1,
    STATIC_COLOR_RANGE,
};

enum LedRingEffectPriority
{
    LED_RING_PRIOTITY_IDLE = 1,
    LED_RING_PRIOTITY_APP = 2,
    LED_RING_PRIOTITY_ERROR = 3,
};

union LedRingEffectPayload
{
    EffectStaticColor static_color;
    EffectStaticColorRange static_color_range;
};

struct LedRingEffect
{
    LedRingEffectPriority priprity;
    LedRingEffectType type;
    unsigned long expire_at;
    LedRingEffectPayload payload;
};

class LedRingTask : public Task<LedRingTask>
{
    friend class Task<LedRingTask>; // Allow base Task to invoke protected run()

public:
    LedRingTask(const uint8_t task_core);
    ~LedRingTask();

    void setLogger(Logger *logger);
    void setEffect(LedRingEffect led_ring_effect);

    LedRingNotifier *getNotifier();

protected:
    void
    run();

private:
    QueueHandle_t render_effect_queue_;

    SemaphoreHandle_t mutex_;
    Logger *logger_;
    void log(const char *msg);

    LedRingNotifier led_ring_notifier_;

    // make layers of priority and timers
    LedRingEffect current_effect_;
    LedRingEffect current_effect_idle_;
    LedRingEffect current_effect_app_;
    LedRingEffect current_effect_error_;

    unsigned long current_effect_last_updated_ms;

    uint32_t effect_timeout_ms;
    unsigned long effect_expiration_ms;

    void renderEffectStaticColor(LedRingEffect effect);
    void renderEffectStaticColorRange(LedRingEffect effect);

    // void renderEffectSnake();
    // void renderEffectLightHouse();
    // void renderTrailEffect();
    // void renderFadeInEffect();
    // void renderFadeOutEffect();
};

#else

class LedRingTask
{
};

#endif
