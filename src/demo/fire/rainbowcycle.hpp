#ifndef DEMO_FIRE_RAINBOWCYCLE_HPP
#define DEMO_FIRE_RAINBOWCYCLE_HPP


#include "renderer/color.hpp"


class RainbowCycle
{
public:

    RainbowCycle();
    ~RainbowCycle();

    Color get_color() const;
    void advance(float step);

    void set_brightness_range(float min, float max, float speed);
    void set_saturation_range(float min, float max, float speed);

private:

    float huePhase;
    float brightPhase;
    float satPhase;

    float minBright, maxBright, speedBright;
    float minSat, maxSat, speedSat;
};


#endif
