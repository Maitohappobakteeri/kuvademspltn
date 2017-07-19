#include "rainbowcycle.hpp"


#include <cmath>


RainbowCycle::RainbowCycle()
    :phase(0)
{

}


RainbowCycle::~RainbowCycle()
{

}



Color RainbowCycle::get_color() const
{
    using std::sin;
    using std::cos;
    return Color{(1.0f + sin(phase*1.25555f))/2.0f,
                 (1.0f + sin(phase*1.20011f))/2.0f,
                 (1.0f + sin(phase*1.333343f))/2.0f};
}


void RainbowCycle::advance(float step)
{
    phase += step;
}
