#pragma once

#include "FreeRTOS.h"
#include "queue.h"

class LedRingNotifier
{
public:
    LedRingNotifier();
    void staticColor(uint32_t color, uint8_t brightness, uint8_t priority);
    void staticColorRange(uint32_t color, uint8_t brightness, uint16_t from_degree, uint16_t to_degree, uint8_t priority);

    // TODO: change this
    QueueHandle_t getQueue();

private:
    QueueHandle_t render_effect_queue_;
};