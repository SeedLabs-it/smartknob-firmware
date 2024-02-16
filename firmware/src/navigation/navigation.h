#pragma once

#include <stdint.h>

const uint8_t NAVIGATION_EVENT_PRESS_NO = 0;
const uint8_t NAVIGATION_EVENT_PRESS_SHORT = 1;
const uint8_t NAVIGATION_EVENT_PRESS_LONG = 2;

struct NavigationEvent
{
    uint8_t press;

    // TODO: should we add rotation also here?
};
