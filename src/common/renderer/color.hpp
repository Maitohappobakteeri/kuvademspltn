#ifndef RENDERER_COLOR_HPP
#define RENDERER_COLOR_HPP


struct Color
{
    float r;
    float g;
    float b;
    float a = 1.0f;

    // common colors
    static Color WHITE;
    static Color BLACK;
    static Color GRAY;
    static Color RED;
    static Color BLUE;
    static Color GREEN;
};


#endif
