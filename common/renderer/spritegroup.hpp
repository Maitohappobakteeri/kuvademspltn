#ifndef RENDERER_SPRITEGROUP_HPP
#define RENDERER_SPRITEGROUP_HPP


#include "sprite.hpp"

#include <set>
#include <boost/pool/object_pool.hpp>


class Renderer;

class SpriteGroup
{
public:
    friend class Renderer;

    SpriteGroup(Renderer* renderer);
    ~SpriteGroup();

    Sprite* create_sprite(const Sprite& sprite);
    void remove_sprite(Sprite* sprite);

    Renderer* get_renderer() const;

    void set_position(const glm::vec2& newPosition);
    glm::vec2 get_position() const;

    void set_scale(const glm::vec2& newScale);
    glm::vec2 get_scale() const;

    // Return this.scale * extendedView
    //     Extends scale to match windows width/height ratio
    glm::vec2 extended_size(const glm::vec2& extendedView) const;

    // Return true if sprite will be visible when rendered
    bool is_sprite_visible(const Sprite& sprite) const;
    bool is_sprite_visible(const Sprite& sprite, const glm::vec2& extendedSize) const;

private:

    Renderer* renderer;
    std::set<Sprite*> sprites;
    boost::object_pool<Sprite> spritePool;

    // values affecting all sprites
    glm::vec2 position;
    glm::vec2 scale;
};

#endif
