#include "string.hpp"


#include "parse.hpp"
#include "error.hpp"


using namespace sklxprs;


String::String()
{

}


String::String(const std::string& str)
    :pureString(parse_string(str))
{

}


String::~String()
{

}


String String::from_pure(const std::string& pureStr)
{
    String string;
    string.pureString = pureStr;
    return string;
}


std::string String::to_string(StringStyles style) const
{
    return format_string(pureString);
}


Object::Type String::get_type() const
{
    return Type::STRING;
}


std::string String::get_pure() const
{
    return pureString;
}


std::string String::format_string(const std::string& pureStr)
{
    const char STRING_CHAR = '"';
    const char ESC_CHAR = '\\';
    const char NEWLINE = '\n';
    const std::string ESCAPABLE_CHARS = std::string(1, STRING_CHAR) + ESC_CHAR + NEWLINE;
    const std::string ESCAPED_STRING_CHAR = "\\\"";
    const std::string ESCAPED_ESC_CHAR = "\\\\";
    const std::string ESCAPED_NEWLINE = "\\n";

    std::string formattedStr = std::string(1, STRING_CHAR);

    // find ESCAPABLE_CHARS and prefix them with ESC_CHAR
    size_t subStartPos = 0;
    size_t escapableCharPos = pureStr.find_first_of(ESCAPABLE_CHARS);
    while(escapableCharPos != std::string::npos)
    {
        // add trivial substring to formattedStr
        formattedStr += pureStr.substr(subStartPos, escapableCharPos - subStartPos);

        // add correct character prefixed with ESC_CHAR
        if(pureStr[escapableCharPos] == STRING_CHAR)
        {
            formattedStr += ESCAPED_STRING_CHAR;
        }
        else if(pureStr[escapableCharPos] == ESC_CHAR)
        {
            formattedStr += ESCAPED_ESC_CHAR;
        }
        else if(pureStr[escapableCharPos] == NEWLINE)
        {
            formattedStr += ESCAPED_NEWLINE;
        }

        // find next escapable character
        subStartPos = escapableCharPos + 1;
        escapableCharPos = pureStr.find_first_of(ESCAPABLE_CHARS, subStartPos);
    }

    // add remaining substr from pureStr
    formattedStr += pureStr.substr(subStartPos);
    formattedStr += std::string(1, STRING_CHAR);

    return formattedStr;
}


std::string String::parse_string(const std::string& formattedStr)
{
    const char STRING_CHAR = '"';
    const char ESC_CHAR = '\\';
    const char NEWLINE = '\n';
    const char NEWLINE_CHAR = 'n';

    // characters we care about finding
    const std::string IMPORTANT_CHARS = std::string(1, STRING_CHAR) + ESC_CHAR;

    // find opening STRING_CHAR
    size_t currentPos = find_and_test_first_not_of(formattedStr, 0, WHITESPACE,
                                                   std::string(1, STRING_CHAR));

    // check if formattedStr is too short
    ++currentPos;
    if(currentPos >= formattedStr.size())
    {
        throw ParsingError("\"", "string end");
    }
    size_t subStartPos = currentPos;

    // parse formattedStr
    std::string pureStr;
    bool isString = true;
    while(isString)
    {
        currentPos = formattedStr.find_first_of(IMPORTANT_CHARS, subStartPos);

        // check if formattedStr ended too soon
        if(currentPos == std::string::npos)
        {
            throw ParsingError("\"", "string end");
        }

        // add trivial substring so currentPos can be changed
        pureStr += formattedStr.substr(subStartPos, currentPos - subStartPos);

        // replace escaped characters or end string
        if(formattedStr[currentPos] == ESC_CHAR)
        {
            if(currentPos + 1 >= formattedStr.size())
            {
                throw ParsingError("escapable char", "string end");
            }
            else if(formattedStr[currentPos + 1] == ESC_CHAR)
            {
                pureStr += ESC_CHAR;
            }
            else if(formattedStr[currentPos + 1] == STRING_CHAR)
            {
                pureStr += STRING_CHAR;
            }
            else if(formattedStr[currentPos + 1] == NEWLINE_CHAR)
            {
                pureStr += NEWLINE;
            }
            else
            {
                throw ParsingError("escapable char", std::string(1, formattedStr[currentPos + 1]));
            }

            subStartPos = currentPos + 2;
        }
        else if(formattedStr[currentPos] == STRING_CHAR)
        {
            isString = false;
        }
    }

    // check that ALL non-whitespace characters were parsed
    size_t lastNotWhitespace = formattedStr.find_last_not_of(WHITESPACE);
    if(currentPos != lastNotWhitespace)
    {
        size_t firstBadPos = formattedStr.find_first_not_of(WHITESPACE, currentPos + 1);
        throw ParsingError("\"", std::string(1, formattedStr[firstBadPos]));
    }

    return pureStr;
}
