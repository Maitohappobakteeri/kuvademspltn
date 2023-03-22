#include "rainbowcycle.hpp"


#include <cmath>


RainbowCycle::RainbowCycle()
    :huePhase(0), brightPhase(0), satPhase(0),
     minBright(1.0f), maxBright(1.0f), speedBright(1.0f),
     minSat(1.0f), maxSat(1.0f), speedSat(1.0f)
{

}


RainbowCycle::~RainbowCycle()
{

}



Color RainbowCycle::get_color() const
{
    using std::sin;

    const float PI = 3.14159265359;
    const float THIRD = 1.0f / 3.0f;
    const float SIXTH = THIRD / 2.0f;

    float red, green, blue;
    if(huePhase >= 2.0f * THIRD)
    {
        if(huePhase >= 2.0f * THIRD + SIXTH)
        {
            red = 1.0f;
            green = 0.0f;
            blue = 1.0f - (huePhase - 2.0f * THIRD - SIXTH) / SIXTH;
        }
        else
        {
            red = (huePhase - 2.0f * THIRD) / SIXTH;
            green = 0.0f;
            blue = 1.0f;
        }
    }
    else if(huePhase >= THIRD)
    {
        if(huePhase >= THIRD + SIXTH)
        {
            red = 0.0f;
            green = 1.0f - (huePhase - THIRD - SIXTH) / SIXTH;
            blue = 1.0f;
        }
        else
        {
            red = 0.0f;
            green = 1.0f;
            blue = (huePhase - THIRD) / SIXTH;
        }
    }
    else
    {
        if(huePhase >= SIXTH)
        {
            red = 1.0f - (huePhase - SIXTH) / SIXTH;
            green = 1.0f;
            blue = 0.0f;
        }
        else
        {
            red = 1.0f;
            green = huePhase / SIXTH;
            blue = 0.0f;
        }
    }

    float saturation = minSat + (maxSat - minSat)
                       * (sin(satPhase * 2.0f * PI) + 1.0f) / 2.0f;
    red += (1.0f - red) * (1.0f - saturation);
    green += (1.0f - green) * (1.0f - saturation);
    blue += (1.0f - blue) * (1.0f - saturation);

    float brightness = minBright + (maxBright - minBright)
                       * (sin(brightPhase * 2.0f * PI) + 1.0f) / 2.0f;
    red *= brightness;
    green *= brightness;
    blue *= brightness;

    return Color{red, green, blue};
}


void RainbowCycle::advance(float step)
{
    huePhase += step;
    while(huePhase >= 1.0f)
    {
        huePhase -= 1.0f;
    }

    brightPhase += step * speedBright;
    while(brightPhase >= 1.0f)
    {
        brightPhase -= 1.0f;
    }

    satPhase += step * speedSat;
    while(satPhase >= 1.0f)
    {
        satPhase -= 1.0f;
    }
}


void RainbowCycle::set_brightness_range(float min, float max, float speed)
{
    minBright = min;
    maxBright = max;
    speedBright = speed;
}


void RainbowCycle::set_saturation_range(float min, float max, float speed)
{
    minSat = min;
    maxSat = max;
    speedSat = speed;
}
