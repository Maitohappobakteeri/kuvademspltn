#ifndef RENDERER_RELATIVE_HPP
#define RENDERER_RELATIVE_HPP


//
// enums for describing values relative to a parent
//


enum class Align
{
    CENTER,
    LEFT,
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT
};


enum class Relative
{
    NONE,
    WIDTH,
    HEIGHT,
    BOTH
};


enum class Offset
{
    NONE,
    WIDTH,
    HEIGHT,
    BOTH
};


#endif
