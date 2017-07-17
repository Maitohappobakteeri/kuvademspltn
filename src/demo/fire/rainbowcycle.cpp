#include "rainbowcycle.hpp"


#include <cmath>


RainbowCycle::RainbowCycle()
{

}


RainbowCycle::~RainbowCycle()
{

}



Color RainbowCycle::get_color() const
{
    using std::sin;
    using std::cos;
    return Color{(1.0f + sin(phase*1.1f))/2.0f,
                 (1.0f + sin(phase*1.2f))/2.0f,
                 (1.0f + sin(phase*1.3f))/2.0f};
}


void RainbowCycle::advance(float step)
{
    phase += step;
}
