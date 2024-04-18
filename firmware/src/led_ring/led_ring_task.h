#pragma once

#if SK_LEDS

#include "../logger.h"
#include "../task.h"
#include "../app_config.h"

const uint8_t total_effects = 1;

struct EffectStatus
{
    uint8_t percent;
    unsigned long last_updated_ms;
};

struct EffectSettings
{
    uint8_t effect_id;
    uint8_t effect_start_pixel;
    uint8_t effect_end_pixel;
    uint8_t effect_accent_pixel;
    uint32_t effect_main_color;
    uint32_t effect_accent_color;
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

    EffectStatus effect_statuses[total_effects];

    void renderEffectSnake();
    void renderEffectStaticColor();
    void renderEffectLightHouse();
    void renderTrailEffect();
    void renderFadeInEffect();
    void renderFadeOutEffect();
};

#else

class LedRingTask
{
};

#endif
