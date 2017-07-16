#ifndef RENDERER_SPRITE_HPP
#define RENDERER_SPRITE_HPP


#include "texture.hpp"

#include <glm/vec2.hpp>

#include <vector>


class Sprite
{
public:

    Sprite(Texture const* texture, const glm::vec2& position, const glm::vec2& scale);
    Sprite(const std::vector<Texture const*>& textures, unsigned int frameDuration,
                   unsigned int syncTime, const glm::vec2& position, const glm::vec2& scale);
    Sprite(std::initializer_list<Texture const*> textures, unsigned int frameDuration,
                   unsigned int syncTime, const glm::vec2& position, const glm::vec2& scale);
    ~Sprite();

    Texture const* get_texture() const;
    glm::vec2 get_position() const;
    glm::vec2 get_scale() const;
    float get_rotation() const;

    void set_position(const glm::vec2& newPosition);
    void set_scale(const glm::vec2& newScale);
    void set_rotation(float newRotation);

    static void set_reference_time(unsigned int timeMs);

private:

    glm::vec2 position;
    glm::vec2 scale;
    float rotation;

    // reference time for calculating the current frame
    static unsigned int timeNow;

    std::vector<Texture const*> textures;

    // time one texture/frame is displayed
    unsigned int frameDuration;
    // animation start time
    unsigned int syncTime;
};

#endif
