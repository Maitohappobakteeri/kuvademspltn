#include "spritegroup.hpp"


#include "renderer.hpp"


SpriteGroup::SpriteGroup(Renderer* renderer)
    :renderer(renderer), position(0,0), scale(1,1)
{

}


SpriteGroup::~SpriteGroup()
{

}


Sprite* SpriteGroup::create_sprite(const Sprite& sprite)
{
    Sprite* newSprite = spritePool.construct(sprite);
    sprites.insert(newSprite);
    return newSprite;
}


void SpriteGroup::remove_sprite(Sprite* sprite)
{
    sprites.erase(sprite);
    spritePool.destroy(sprite);
}


Renderer* SpriteGroup::get_renderer() const
{
    return renderer;
}


void SpriteGroup::set_position(const glm::vec2& newPosition)
{
    position = newPosition;
}


glm::vec2 SpriteGroup::get_position() const
{
    return position;
}


void SpriteGroup::set_scale(const glm::vec2& newScale)
{
    scale = newScale;
}


glm::vec2 SpriteGroup::get_scale() const
{
    return scale;
}


glm::vec2 SpriteGroup::extended_size(const glm::vec2& extendedView) const
{
    return scale * extendedView;
}


bool SpriteGroup::is_sprite_visible(const Sprite& sprite) const
{
    if(position.x + scale.x <= sprite.get_position().x - sprite.get_scale().x) return false;
    if(position.x - scale.x >= sprite.get_position().x + sprite.get_scale().x) return false;
    if(position.y + scale.y <= sprite.get_position().y - sprite.get_scale().y) return false;
    if(position.y - scale.y >= sprite.get_position().y + sprite.get_scale().y) return false;
    return true;
}


bool SpriteGroup::is_sprite_visible(const Sprite& sprite, const glm::vec2& extendedSize) const
{
    if(position.x + extendedSize.x <= sprite.get_position().x - sprite.get_scale().x) return false;
    if(position.x - extendedSize.x >= sprite.get_position().x + sprite.get_scale().x) return false;
    if(position.y + extendedSize.y <= sprite.get_position().y - sprite.get_scale().y) return false;
    if(position.y - extendedSize.y >= sprite.get_position().y + sprite.get_scale().y) return false;
    return true;
}
