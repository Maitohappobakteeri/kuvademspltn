#ifndef RENDERER_SPRITEMODEL_HPP
#define RENDERER_SPRITEMODEL_HPP


#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"
#include "renderer/sprite.hpp"

#include <vector>
#include <string>


struct SpriteModel
{
    std::vector<std::shared_ptr<Texture>> textures;
    unsigned int frameDuration;
    unsigned int syncTime;
    bool useCurrentTime;
};


#endif
