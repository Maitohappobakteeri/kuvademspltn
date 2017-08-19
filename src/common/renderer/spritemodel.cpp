#include "spritemodel.hpp"


Sprite create_sprite(const SpriteModel& spriteData)
{
    std::vector<Texture const*> spriteTextures;
    for(const std::shared_ptr<Texture>& sharedPtr : spriteData.textures)
    {
        spriteTextures.push_back(sharedPtr.get());
    }

    if(spriteData.useCurrentTime)
    {
        return Sprite(spriteTextures, spriteData.frameDuration, SDL_GetTicks(), {0,0}, {1,1},
                      spriteData.loop);
    }
    else
    {
        return Sprite(spriteTextures, spriteData.frameDuration, spriteData.syncTime, {0,0}, {1,1},
                      spriteData.loop);
    }
}
