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

lv_color_t kelvinToLvColor(int16_t kelvin)
{
    float temp = kelvin / 100;
    float red, green, blue;

    if (temp <= 66)
    {
        red = 255;
        green = temp;
        green = 99.4708025861 * log(green) - 161.1195681661;

        if (temp <= 19)
        {
            blue = 0;
        }
        else
        {
            blue = temp - 10;
            blue = 138.5177312231 * log(blue) - 305.0447927307;
        }
    }
    else
    {
        red = temp - 60;
        red = 329.698727446 * pow(red, -0.1332047592);
        green = temp - 60;
        green = 288.1221695283 * pow(green, -0.0755148492);
        blue = 255;
    }

    return LV_COLOR_MAKE(CLAMP<uint8_t>(red, 0, 255), CLAMP<uint8_t>(green, 0, 255), CLAMP<uint8_t>(blue, 0, 255));
}