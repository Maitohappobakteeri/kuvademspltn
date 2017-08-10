#include "sprite.hpp"

unsigned int Sprite::timeNow = 0;


Sprite::Sprite(Texture const* texture, const glm::vec2& position, const glm::vec2& scale)
    :position(position), scale(scale), rotation(0), textures(), frameDuration(999),
     syncTime(0), loop(true)
{
    textures.push_back(texture);
}


Sprite::Sprite(const std::vector<Texture const*>& textures,
               unsigned int frameDuration, unsigned int syncTime,
               const glm::vec2& position, const glm::vec2& scale, bool loop)
   :position(position), scale(scale), rotation(0), textures(textures),
    frameDuration(frameDuration), syncTime(syncTime), loop(loop)
{

}


Sprite::Sprite(std::initializer_list<Texture const*> textures,
               unsigned int frameDuration, unsigned int syncTime,
               const glm::vec2& position, const glm::vec2& scale, bool loop)
   :position(position), scale(scale), rotation(0), textures(textures),
    frameDuration(frameDuration), syncTime(syncTime), loop(loop)
{

}


Sprite::~Sprite()
{

}


Texture const* Sprite::get_texture() const
{
    if(textures.size() == 1)
    {
        return *textures.begin();
    }
    else
    {
        unsigned int frameIndex = ((timeNow - syncTime) / frameDuration);
        if(loop)
        {
            return textures[frameIndex % textures.size()];
        }
        else
        {
            if(frameIndex >= textures.size())
            {
                return textures.back();
            }
            else
            {
                return textures[frameIndex];
            }
        }
    }
}


glm::vec2 Sprite::get_position() const
{
    return position;
}


glm::vec2 Sprite::get_scale() const
{
    return scale;
}


float Sprite::get_rotation() const
{
    return rotation;
}

void Sprite::set_position(const glm::vec2& newPosition)
{
    position = newPosition;
}


void Sprite::set_scale(const glm::vec2& newScale)
{
    scale = newScale;
}


void Sprite::set_rotation(float newRotation)
{
    rotation = newRotation;
}

void Sprite::set_reference_time(unsigned int timeMs)
{
    timeNow = timeMs;
}
