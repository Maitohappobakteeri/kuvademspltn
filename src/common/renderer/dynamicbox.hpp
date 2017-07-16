#ifndef RENDERER_DYNAMICBOX_HPP
#define RENDERER_DYNAMICBOX_HPP


#include "glm/glm.hpp"


class Renderer;


class DynamicBox
{
public:

    enum Align
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

    enum Relative
    {
        REL_NONE,
        REL_WIDTH,
        REL_HEIGHT,
        REL_BOTH
    };

    enum Offset
    {
        OFF_NONE,
        OFF_WIDTH,
        OFF_HEIGHT,
        OFF_BOTH
    };

    struct DynamicVec
    {
        glm::vec2 baseVec;
        Relative relative;
        Offset offsetMode;

        glm::vec2 result;
    };

    DynamicBox();
    ~DynamicBox();

    // update results
    void update(const Renderer& renderer);
    void update(const DynamicBox& dbox);

    // Return position from inside the box
    glm::vec2 box_position(const glm::vec2 vec) const;

    // Return vec relative to box scale
    glm::vec2 box_scale(const glm::vec2 vec) const;

    void set_align(Align newAlign);
    Align get_align() const;

    // Set position baseVec, relative and offsetMode
    void set_position(const glm::vec2& newPosition, Relative rel=REL_BOTH, Offset offset=OFF_NONE);

    // Return position.result
    glm::vec2 get_position() const;

    // Set scale baseVec, relative and offsetMode
    void set_scale(const glm::vec2& newScale, Relative rel=REL_BOTH, Offset offset=OFF_NONE);

    // Return scale.result
    glm::vec2 get_scale() const;

private:

    void update(const glm::vec2& positionOffset, const glm::vec2& viewScale);

    Align align;
    DynamicVec position;
    DynamicVec scale;
};


#endif
