#include "dynamicbox.hpp"


#include "renderer.hpp"


DynamicBox::DynamicBox()
    :align(CENTER),
    position{{0,0}, REL_BOTH, OFF_NONE, {0,0}},
    scale{{1,1}, REL_BOTH, OFF_NONE, {1,1}}
{

}


DynamicBox::~DynamicBox()
{

}


void DynamicBox::update(const Renderer& renderer)
{
    update(glm::vec2(0, 0), renderer.get_view_scale());
}


void DynamicBox::update(const DynamicBox& dbox)
{
    update(dbox.get_position(), dbox.get_scale());
}


void DynamicBox::update(const glm::vec2& positionOffset, const glm::vec2& viewScale)
{
    // scale
    //
    if(scale.offsetMode == OFF_BOTH)
    {
        scale.result = viewScale + scale.result;
    }
    else if(scale.offsetMode == OFF_WIDTH)
    {
        scale.result.x = viewScale.x + scale.baseVec.x;
        scale.result.y = scale.baseVec.y;
    }
    else if(scale.offsetMode == OFF_HEIGHT)
    {
        scale.result.x = scale.baseVec.x;
        scale.result.y = viewScale.y + scale.baseVec.y;
    }
    else
    {
        scale.result = scale.baseVec;
    }

    if(scale.relative == REL_BOTH)
    {
        scale.result = scale.result * viewScale;
    }
    else if(scale.relative == REL_WIDTH)
    {
        scale.result.x = scale.result.x * viewScale.x;
    }
    else if(scale.relative == REL_HEIGHT)
    {
        scale.result.y = scale.result.y * viewScale.y;
    }

    // position
    //
    if(position.offsetMode == OFF_BOTH)
    {
        position.result = viewScale + position.result;
    }
    else if(position.offsetMode == OFF_WIDTH)
    {
        position.result.x = viewScale.x + position.baseVec.x;
        position.result.y = position.baseVec.y;
    }
    else if(position.offsetMode == OFF_HEIGHT)
    {
        position.result.x = position.baseVec.x;
        position.result.y = viewScale.y + position.baseVec.y;
    }
    else
    {
        position.result = position.baseVec;
    }

    if(position.relative == REL_BOTH)
    {
        position.result = position.result * viewScale;
    }
    else if(position.relative == REL_WIDTH)
    {
        position.result.x = position.result.x * viewScale.x;
    }
    else if(position.relative == REL_HEIGHT)
    {
        position.result.y = position.result.y * viewScale.y;
    }

    // align horizontal
    if(align == LEFT || align == TOP_LEFT || align == BOTTOM_LEFT)
    {
        position.result.x -= viewScale.x - scale.result.x;
    }
    else if(align == RIGHT || align == TOP_RIGHT || align == BOTTOM_RIGHT)
    {
        position.result.x += viewScale.x - scale.result.x;
    }
    // vertical
    if(align == TOP || align == TOP_LEFT || align == TOP_RIGHT)
    {
        position.result.y += viewScale.y - scale.result.y;
    }
    else if(align == BOTTOM || align == BOTTOM_LEFT || align == BOTTOM_RIGHT)
    {
        position.result.y -= viewScale.y - scale.result.y;
    }

    // move position.result
    position.result += positionOffset;
}


glm::vec2 DynamicBox::box_position(const glm::vec2 vec) const
{
    return position.result + vec * scale.result;
}


glm::vec2 DynamicBox::box_scale(const glm::vec2 vec) const
{
    return vec * scale.result;
}


void DynamicBox::set_align(DynamicBox::Align newAlign)
{
    align = newAlign;
}


DynamicBox::Align DynamicBox::get_align() const
{
    return align;
}


void DynamicBox::set_position(const glm::vec2& newPosition, DynamicBox::Relative rel,
                              Offset offset)
{
    position.baseVec = newPosition;
    position.relative = rel;
    position.offsetMode = offset;
}


glm::vec2 DynamicBox::get_position() const
{
    return position.result;
}


void DynamicBox::set_scale(const glm::vec2& newScale, DynamicBox::Relative rel, Offset offset)
{
    scale.baseVec = newScale;
    scale.relative = rel;
    scale.offsetMode = offset;
}


glm::vec2 DynamicBox::get_scale() const
{
    return scale.result;
}
