#include "util.h"

float lerp(const float value, const float inMin, const float inMax, const float min, const float max)
{
    // Map the input value from the input range to the output range
    return ((value - inMin) / (inMax - inMin)) * (max - min) + min;
}

MovingAverage::MovingAverage(int filterLength)
{
    this->filterLength = filterLength;
    this->filterPointer = new int32_t[filterLength];
    this->lastValue = 0.0;
    initFilter();
}

int32_t MovingAverage::addSample(int32_t newValue)
{
    shiftFilter(newValue);
    computeAverage();
    return this->lastValue;
}

int32_t MovingAverage::getValue()
{
    return this->lastValue;
}

void MovingAverage::dumpFilter()
{
    this->lastValue = 0.0;
    initFilter();
}

void MovingAverage::shiftFilter(int32_t nextValue)
{
    for (int i = this->filterLength - 1; i > -1; i--)
    {
        if (i == 0)
        {
            *(filterPointer) = nextValue;
        }
        else
        {
            *(filterPointer + i) = *(filterPointer + (i - 1));
        }
    }
}

void MovingAverage::computeAverage()
{
    double sum = 0.0;
    for (int i = 0; i < this->filterLength; i++)
    {
        sum += *(filterPointer + i);
    }
    this->lastValue = sum / this->filterLength;
}

void MovingAverage::initFilter()
{
    for (int i = 0; i < this->filterLength; i++)
    {
        *(filterPointer + i) = 0.0;
    }
}