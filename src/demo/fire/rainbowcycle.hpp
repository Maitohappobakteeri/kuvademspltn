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

private:

    float phase;
};


#endif
