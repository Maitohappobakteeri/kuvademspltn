#include "loadsprite.hpp"


#include "strkilxprs/strkilxprs.hpp"

#include <SDL/SDL.h>

#include <regex>
#include <fstream>
#include <streambuf>
#include <limits>


namespace prs = sklxprs;


namespace
{
    double string_to_double(const std::string& str)
    {
        const std::string errorMessage = "failed to convert to double";

        size_t readAmount;
        double newDouble;
        try
        {
            newDouble = std::stod(str, &readAmount);
        }
        catch(std::invalid_argument& e)
        {
            throw std::runtime_error(errorMessage);
        }
        catch(std::out_of_range& e)
        {
            throw std::runtime_error(errorMessage);
        }
        if(readAmount != str.size())
        {
            throw std::runtime_error(errorMessage);
        }

        return newDouble;
    }


    double string_to_uint(const std::string& str)
    {
        const std::string errorMessage = "failed to convert to uint";

        size_t readAmount;
        unsigned long newULong;
        try
        {
            newULong = std::stoul(str, &readAmount);
        }
        catch(std::invalid_argument& e)
        {
            throw std::runtime_error(errorMessage);
        }
        catch(std::out_of_range& e)
        {
            throw std::runtime_error(errorMessage);
        }
        if(readAmount != str.size())
        {
            throw std::runtime_error(errorMessage);
        }
        else if(newULong >= (unsigned long) std::numeric_limits<unsigned int>::max())
        {
            throw std::runtime_error(errorMessage);
        }

        return (unsigned int) newULong;
    }
}


SpriteModel parse_spritedata(Renderer* renderer, const std::string& spriteString)
{
    // create Map from spriteString
    prs::Map map(spriteString);
    SpriteModel spriteData;

    // get textures list
    prs::Object* object = map.get("textures");
    if(object == nullptr)
    {
        throw std::runtime_error("\"textures\" list doesn't exist");
    }
    else if(object->get_type() != prs::Object::Type::LIST)
    {
        throw std::runtime_error("\"textures\" object isn't a list");
    }
    prs::List* textureList = static_cast<prs::List*>(map.get("textures"));

    // push paths from textureList to vector
    for(unsigned int i = 0; i < textureList->size(); ++i)
    {
        spriteData.textures.push_back(renderer->load_texture(textureList->get(i)->to_string()));
    }
    if(spriteData.textures.size() == 0)
    {
        throw std::runtime_error("\"textures\" list is empty");
    }

    // get frameDuration unsigned long
    prs::Object* frameDuration = map.get("frameDuration");
    if(frameDuration != nullptr)
    {
        try
        {
            spriteData.frameDuration = string_to_uint(frameDuration->to_string());
        }
        catch(std::runtime_error& e)
        {
            throw std::runtime_error(std::string("failed to read ")
                                     + "frameDuration" + ": "
                                     + e.what());
        }
    }

    // get frameDuration unsigned long
    prs::Object* syncTime = map.get("syncTime");
    if(syncTime != nullptr)
    {
        if(syncTime->to_string() == "CURRENT")
        {
            spriteData.useCurrentTime = true;
        }
        else
        {
            spriteData.useCurrentTime = false;
            try
            {
                spriteData.syncTime = string_to_uint(syncTime->to_string());
            }
            catch(std::runtime_error& e)
            {
                throw std::runtime_error(std::string("failed to read ")
                                         + "syncTime" + ": "
                                         + e.what());
            }
        }
    }

    return spriteData;
}


SpriteModel read_spritedata_from_file(Renderer* renderer, const std::string& filePath)
{
    try
    {
        return parse_spritedata(renderer, prs::read_full_file(filePath));
    }
    catch(std::runtime_error& e)
    {
        throw std::runtime_error(std::string("file: \"") + filePath
                                 + "\" -> "
                                 + e.what());
    }
}


Sprite create_sprite(const SpriteModel& spriteData)
{
    std::vector<Texture const*> spriteTextures;
    for(const std::shared_ptr<Texture>& sharedPtr : spriteData.textures)
    {
        spriteTextures.push_back(sharedPtr.get());
    }

    if(spriteData.useCurrentTime)
    {
        return Sprite(spriteTextures, spriteData.frameDuration, SDL_GetTicks(), {0,0}, {1,1});
    }
    else
    {
        return Sprite(spriteTextures, spriteData.frameDuration, spriteData.syncTime, {0,0}, {1,1});
    }
}
