#include "object.hpp"


#include "parse.hpp"
#include "plain.hpp"
#include "list.hpp"
#include "map.hpp"
#include "string.hpp"


using namespace sklxprs;


Object::Object()
{

}


Object::~Object()
{

}


Object::operator std::string() const
{
    return to_string();
}


StringStyles Object::get_preferred_string_style() const
{
    return SPACED;
}


Object* sklxprs::create_object(const std::string& str)
{
    size_t firstNonWhitespace = str.find_first_not_of(WHITESPACE);
    if(firstNonWhitespace == std::string::npos)
    {
        return new Plain();
    }
    else if(str[firstNonWhitespace] == '{')
    {
        return new Map(str);
    }
    else if(str[firstNonWhitespace] == '[')
    {
        return new List(str);
    }
    else if(str[firstNonWhitespace] == '"')
    {
        return new String(str);
    }
    else
    {
        return new Plain(str);
    }
}
