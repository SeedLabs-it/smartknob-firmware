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

uint32_t rgbToUint32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Converts a color from HSVA to RGBA
uint32_t ToRGBA(float h)
{
    float s = 1;
    float a = 1;
    float v = 255;

    float r, g, b = 0;

    // Achromatic (gray)
    if (s == 0)
    {
        r = g = b = v / 255.0f;
        a = a;
        return rgbToUint32(uint8_t(r), uint8_t(g), uint8_t(b));
    }

    // Conversion values
    float tempH = h;
    tempH /= 60.0f;
    int i = (int)std::floor(tempH);
    float f = tempH - i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    // There are 6 cases, one for every 60 degrees
    switch (i)
    {
    case 0:
        r = v;
        g = t;
        b = p;
        break;

    case 1:
        r = q;
        g = v;
        b = p;
        break;

    case 2:
        r = p;
        g = v;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = v;
        break;

    case 4:
        r = t;
        g = p;
        b = v;
        break;

    // Case 5
    default:
        r = v;
        g = p;
        b = q;
        break;
    }

    // Alpha value stays the same
    a = a;

    // ESP_LOGD("color", "%.0f %.0f %.0f %.0f", h, r, g, b);

    return rgbToUint32(uint8_t(r), uint8_t(g), uint8_t(b));
}