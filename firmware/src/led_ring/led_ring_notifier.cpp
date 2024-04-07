#include "led_ring_notifier.h"

// should be here to avoid circular references
#include "led_ring_task.h"

LedRingNotifier::LedRingNotifier()
{
    render_effect_queue_ = xQueueCreate(10, sizeof(LedRingEffect));
}

QueueHandle_t LedRingNotifier::getQueue()
{
    return render_effect_queue_;
}

void LedRingNotifier::staticColor(uint32_t color, uint8_t brightness, uint8_t priority)
{
    LedRingEffect led_ring_effect;

    led_ring_effect.type = STATIC_COLOR;
    led_ring_effect.priprity = LedRingEffectPriority(priority);
    led_ring_effect.expire_at = millis() + 200;
    led_ring_effect.payload.static_color.color = color;
    led_ring_effect.payload.static_color.brightness = brightness;

    xQueueSend(render_effect_queue_, &led_ring_effect, 0);
}

void LedRingNotifier::staticColorRange(uint32_t color, uint8_t brightness, uint16_t from_degree, uint16_t to_degree, uint8_t priority)
{
    LedRingEffect led_ring_effect;

    led_ring_effect.type = STATIC_COLOR_RANGE;
    led_ring_effect.priprity = LedRingEffectPriority(priority);
    led_ring_effect.expire_at = millis() + 200;
    led_ring_effect.payload.static_color_range.color = color;
    led_ring_effect.payload.static_color_range.brightness = brightness;
    led_ring_effect.payload.static_color_range.from_degrees = from_degree;
    led_ring_effect.payload.static_color_range.to_degrees = to_degree;

    xQueueSend(render_effect_queue_, &led_ring_effect, 0);
}
