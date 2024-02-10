#pragma once

#include <stdint.h>
#include "Arduino.h"

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
  int32_t addSample(int32_t newValue);
  int32_t getValue();
  void dumpFilter();

private:
  int32_t *filterPointer;
  int filterLength;
  int32_t lastValue;

  void initFilter();
  void shiftFilter(int32_t nextValue);
  void computeAverage();
};

uint32_t ToRGBA(float h);
uint32_t rgbToUint32(uint8_t r, uint8_t g, uint8_t b);