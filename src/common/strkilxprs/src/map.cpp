#include "map.hpp"


#include "parse.hpp"
#include "plain.hpp"
#include "error.hpp"


using namespace sklxprs;


Map::Map()
{

}


Map::Map(const std::string& str)
{
    // check str contains only whitespace
    size_t currentPos = str.find_first_not_of(WHITESPACE);
    if(currentPos == std::string::npos)
    {
        return;
    }

    // check if starts with { and ends with }
    bool startWithCurly = false;
    if(str[currentPos] == '{')
    {
        startWithCurly = true;
        ++currentPos;
    }

    while(true)
    {
        // check if Map ends here
        currentPos = str.find_first_not_of(WHITESPACE, currentPos);
        if(currentPos == std::string::npos)
        {
            if(startWithCurly)
            {
                throw ParsingError("}", "string end");
            }
            else
            {
                break;
            }
        }
        else if(startWithCurly && str[currentPos] == '}')
        {
            break;
        }

        // set key start and find key end
        size_t keyStartPos = currentPos;
        currentPos = str.find_first_not_of(VARIABLE_CHARACTERS, currentPos);
        if(currentPos == std::string::npos)
        {
            throw ParsingError("=", "string end");
        }

        // take key substring
        std::string keyName = str.substr(keyStartPos, currentPos - keyStartPos);
        if(keyName.size() == 0)
        {
            throw ParsingError("key name", std::string(1, str[currentPos]));
        }

        // check for =
        currentPos = find_and_test_first_not_of(str, currentPos, WHITESPACE, "=");
        ++currentPos;

        // value start
        currentPos = str.find_first_not_of(WHITESPACE, currentPos);
        if(currentPos == std::string::npos)
        {
            throw ParsingError("object", "string end");
        }
        size_t valueStartPos = currentPos;

        // value end
        currentPos = find_object_end(str, currentPos, ";");

        // take value substring
        std::string itemStr = str.substr(valueStartPos, currentPos - valueStartPos);
        if(itemStr.size() == 0)
        {
            throw ParsingError("object", std::string(1, str[currentPos]));
        }

        // check for ;
        currentPos = find_and_test_first_not_of(str, currentPos, WHITESPACE, ";");
        ++currentPos;

        // create and assign new object
        items[keyName] = std::unique_ptr<Object>(create_object(itemStr));
    }
}


Map::~Map()
{

}


unsigned int Map::size() const
{
    return items.size();
}


void Map::clear()
{
    items.clear();
}


Object* Map::get(const std::string& key)
{
    auto it = items.find(key);
    return (it == items.end() ? nullptr : it->second.get());
}


Object const* Map::get(const std::string& key) const
{
    auto it = items.find(key);
    return (it == items.end() ? nullptr : it->second.get());
}


void Map::remove(const std::string& key)
{
    auto it = items.find(key);
    if(it == items.end())
    {
        throw std::out_of_range("key doesn't exist");
    }

    items.erase(it);
}


std::string Map::to_string(StringStyles style) const
{
    if(style == PREFERRED)
    {
        style = get_preferred_string_style();
    }

    if(style == TIGHT)
    {
        if(size() == 0)
        {
            return std::string("{}");
        }
        else
        {
            std::string str = std::string("{");
            for(auto& pair : items)
            {
                str += pair.first;
                str += "=";
                str += pair.second->to_string();
                str += ";";
            }
            str += "}";
            return str;
        }
    }
    else if(style == SPACED)
    {
        if(size() == 0)
        {
            return std::string("{ }");
        }
        else
        {
            std::string str = std::string("{ ");
            for(auto& pair : items)
            {
                str += pair.first;
                str += " = ";
                str += pair.second->to_string();
                str += "; ";
            }
            str += "}";
            return str;
        }
    }
    else if(style == LINES)
    {
        if(size() == 0)
        {
            return std::string("{\n\n}");
        }
        else
        {
            std::string str = std::string("{\n");
            for(auto& pair : items)
            {
                str += INDENTATION;
                str += pair.first;
                str += " = ";

                std::string valueStr = pair.second->to_string();

                size_t newlinePos = valueStr.find("\n");
                str += valueStr.substr(0, newlinePos);

                while(newlinePos != std::string::npos)
                {
                    size_t lineStart = newlinePos + 1;
                    newlinePos = valueStr.find("\n", lineStart);

                    str += "\n";
                    str += INDENTATION;
                    str += valueStr.substr(lineStart, newlinePos - lineStart);
                }
                str += ";\n";
            }
            str += "}";
            return str;
        }
    }
    else if(style == MORE_LINES)
    {
        if(size() == 0)
        {
            return std::string("{\n\n}");
        }
        else
        {
            std::string str = std::string("{\n\n");
            for(auto& pair : items)
            {
                str += INDENTATION;
                str += pair.first;
                str += " = ";

                std::string valueStr = pair.second->to_string();

                size_t newlinePos = valueStr.find("\n");
                str += valueStr.substr(0, newlinePos);

                while(newlinePos != std::string::npos)
                {
                    size_t lineStart = newlinePos + 1;
                    newlinePos = valueStr.find("\n", lineStart);

                    str += "\n";
                    str += INDENTATION;
                    str += valueStr.substr(lineStart, newlinePos - lineStart);
                }
                str += ";\n\n";
            }
            str += "}";
            return str;
        }
    }

    throw std::runtime_error("this should never be reached");
}


StringStyles Map::get_preferred_string_style() const
{
    return LINES;
}


Object::Type Map::get_type() const
{
    return Type::MAP;
}
