#include "plain.hpp"


#include "parse.hpp"
#include "error.hpp"


using namespace sklxprs;


Plain::Plain()
{

}


Plain::Plain(const std::string& str)
    :content()
{
    size_t firstNotWhitespace = str.find_first_not_of(WHITESPACE);
    if(firstNotWhitespace == std::string::npos)
    {
        throw ParsingError("plain object", "string end");
    }

    size_t plainObjectEnd = find_plain_end(str, firstNotWhitespace, "");

    size_t lastNotWhitespace = str.find_last_not_of(WHITESPACE);
    if(plainObjectEnd != lastNotWhitespace + 1)
    {
        throw ParsingError("string end", std::string(1, str[plainObjectEnd]));
    }

    content = str.substr(firstNotWhitespace, plainObjectEnd - firstNotWhitespace);
}


Plain::~Plain()
{

}


std::string Plain::to_string(StringStyles style) const
{
    return content;
}


Object::Type Plain::get_type() const
{
    return Type::PLAIN;
}
