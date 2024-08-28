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

HEXColor hToHEX(float h)
{
    float s = 1;
    float a = 1;
    float v = 255;

    float r, g, b = 0;

    HEXColor result;

    // Achromatic (gray)
    if (s == 0)
    {
        r = g = b = v / 255.0f;
        a = a;

        result.r = r;
        result.g = g;
        result.b = b;

        return result;
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

    result.r = r;
    result.g = g;
    result.b = b;

    return result;
}

HSVColor ToHSV(RGBColor_Custom color)
{
    double cmax = std::max({color.r, color.g, color.b});
    double cmin = std::min({color.r, color.g, color.b});
    double delta = cmax - cmin;

    double h = 0, s = 0, v = 0;

    // Calculating hue
    if (delta == 0)
        h = 0;
    else if (cmax == color.r)
        h = 60 * ((color.g - color.b) / delta);
    else if (cmax == color.g)
        h = 60 * (((color.b - color.r) / delta) + 2);
    else if (cmax == color.b)
        h = 60 * (((color.r - color.g) / delta) + 4);

    if (h < 0)
        h += 360;

    // Calculating saturation
    if (cmax != 0)
        s = delta / cmax;

    // Calculating value
    v = cmax;
    return {h, s, v};
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

    return rgbToUint32(uint8_t(r), uint8_t(g), uint8_t(b));
}