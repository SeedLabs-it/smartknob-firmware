#pragma once

#include <stdint.h>
#include "Arduino.h"
#include "lvgl.h"

lv_obj_t *lvDrawCircle(lv_obj_t *parent, const uint8_t dia);

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

struct HSVColor
{
    float h;
    float s;
    float v;
};

struct RGBColor
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct HEXColor
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

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

HEXColor hToHEX(float h);
HSVColor ToHSV(RGBColor color);
uint32_t ToRGBA(float h);
uint32_t rgbToUint32(uint8_t r, uint8_t g, uint8_t b);
RGBColor uint32ToRGB(uint32_t color);
