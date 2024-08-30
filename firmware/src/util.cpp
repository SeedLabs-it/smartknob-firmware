#include "util.h"

lv_obj_t *
lvDrawCircle(const uint8_t dia, lv_obj_t *parent)
{
    static lv_style_t style;
    lv_style_set_radius(&style, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);

    lv_obj_t *circle = lv_obj_create(parent);
    lv_obj_remove_style_all(circle);
    lv_obj_set_size(circle, dia, dia);
    lv_obj_add_style(circle, &style, 0);
    return circle;
}

float lerp(const float value, const float inMin, const float inMax, const float min, const float max)
{
    // Map the input value from the input range to the output range
    return ((value - inMin) / (inMax - inMin)) * (max - min) + min;
}

MovingAverage::MovingAverage(int filterLength)
{
    this->filterLength = filterLength;
    this->filterPointer = new float[filterLength];
    this->lastValue = 0.0;
    initFilter();
}

float MovingAverage::addSample(float newValue)
{
    shiftFilter(newValue);
    computeAverage();
    return this->lastValue;
}

float MovingAverage::getValue()
{
    return this->lastValue;
}

void MovingAverage::dumpFilter()
{
    this->lastValue = 0.0;
    initFilter();
}

void MovingAverage::shiftFilter(float nextValue)
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
