#pragma once

#include <stdint.h>
#include "Arduino.h"
#include "lvgl.h"

// ? MOVE STYLE STUFF TO SEPERATE FILE

const lv_color_t dark_arc_bg = LV_COLOR_MAKE(0x21, 0x21, 0x21);

// WRONG ORDER OF INITIALIZERS IN ORIGINAL MACRO PROVIDED BY LVGL
#ifdef LV_STYLE_CONST_INIT
#undef LV_STYLE_CONST_INIT
#endif

#define LV_STYLE_CONST_INIT(var_name, prop_array)                   \
    const lv_style_t var_name = {                                   \
        .v_p = {.const_props = prop_array},                         \
        .prop1 = LV_STYLE_PROP_ANY,                                 \
        .has_group = 0xFF,                                          \
        .prop_cnt = (sizeof(prop_array) / sizeof((prop_array)[0])), \
    }
// static lv_style_t x20_icon_style;

const lv_style_const_prop_t x20_icon_style_props[] = {
    LV_STYLE_CONST_IMG_RECOLOR_OPA(LV_OPA_COVER),
    LV_STYLE_CONST_IMG_RECOLOR(LV_COLOR_MAKE(0x47, 0x47, 0x47)),
    LV_STYLE_PROP_INV};

LV_STYLE_CONST_INIT(SK_X20_ICON_STYLE, x20_icon_style_props);

lv_obj_t *lvDrawCircle(const uint8_t dia, lv_obj_t *parent = NULL);

template <typename T>
T CLAMP(const T &value, const T &low, const T &high)
{
    return value < low ? low : (value > high ? high : value);
}

#define COUNT_OF(A) (sizeof(A) / sizeof(A[0]))

float lerp(const float value, const float inMin, const float inMax, const float min, const float max);

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

// source: https://github.com/careyi3/MovingAverage/blob/master/src/MovingAverage.cpp
class MovingAverage
{
public:
    MovingAverage(int filterLength);
    float addSample(float newValue);
    float getValue();
    void dumpFilter();

private:
    float *filterPointer;
    int filterLength;
    float lastValue;

    void initFilter();
    void shiftFilter(float nextValue);
    void computeAverage();
};

// FOR DEBUGGING
#define LOG_STACK() LOGD("FREE STACK MEM: %d", uxTaskGetStackHighWaterMark(NULL))

lv_color_t kelvinToLvColor(int16_t kelvin);

lv_color_t kelvinToLvColor(int16_t kelvin);
// FOR DEBUGGING
#define LOG_STACK() LOGD("FREE STACK MEM: %d", uxTaskGetStackHighWaterMark(NULL))