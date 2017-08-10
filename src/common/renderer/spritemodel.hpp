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
    unsigned int frameDuration = 100;
    unsigned int syncTime = 0;
    bool useCurrentTime = false;
    bool loop = true;
};


#endif
