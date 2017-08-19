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


SpriteModel parse_spritemodel(Renderer* renderer, const std::string& spriteString)
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

    // get syncTime unsigned long
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


    // get loop bool
    prs::Object* loopObj = map.get("loop");
    if(loopObj != nullptr)
    {
        std::string loopStr = loopObj->to_string();
        if(loopStr == "true")
        {
            spriteData.loop = true;
        }
        else if(loopStr == "false")
        {
            spriteData.loop = false;
        }
        else
        {
            throw std::runtime_error(std::string("failed to read ") + "loop bool");
        }
    }


    return spriteData;
}


SpriteModel load_spritemodel_from_file(Renderer* renderer, const std::string& filename)
{
    try
    {
        return parse_spritemodel(renderer, prs::read_full_file(filename));
    }
    catch(std::runtime_error& e)
    {
        throw std::runtime_error(std::string("file: \"") + filename
                                 + "\" -> "
                                 + e.what());
    }
}
