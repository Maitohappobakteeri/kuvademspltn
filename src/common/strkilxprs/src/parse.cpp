#include "parse.hpp"


#include "object.hpp"
#include "list.hpp"
#include "map.hpp"
#include "parse.hpp"
#include "error.hpp"


size_t sklxprs::find_object_end(const std::string& str, size_t objectStart,
                                const std::string& delim)
{
    const char STRING_CHAR = '"';
    const char MAP_CHAR = '{';
    const char LIST_CHAR = '[';

    size_t firstNotWhitespace = str.find_first_not_of(WHITESPACE, objectStart);
    if(firstNotWhitespace == std::string::npos)
    {
        throw ParsingError("object", "string end");
    }
    else if(str[firstNotWhitespace] == STRING_CHAR)
    {
        return find_string_end(str, firstNotWhitespace);
    }
    else if(str[firstNotWhitespace] == MAP_CHAR)
    {
        return find_map_end(str, firstNotWhitespace);
    }
    else if(str[firstNotWhitespace] == LIST_CHAR)
    {
        return find_list_end(str, firstNotWhitespace);
    }
    else
    {
        return find_plain_end(str, firstNotWhitespace, delim);
    }
}


size_t sklxprs::find_string_end(const std::string& str, size_t stringStart)
{
    const char STRING_CHAR = '"';
    const char ESC_CHAR = '\\';

    // check for opening STRING_CHAR
    size_t currentPos = str.find_first_not_of(WHITESPACE, stringStart);
    if(currentPos == std::string::npos || str[currentPos] != STRING_CHAR)
    {
        throw ParsingError(std::string(1, STRING_CHAR), "string end");
    }
    ++currentPos;

    bool isString = true;
    bool escapeNext = false;
    while(isString && currentPos < str.size())
    {
        const char c = str[currentPos];

        if(!escapeNext)
        {
            if(c == STRING_CHAR)
            {
                isString = false;
            }
            else if(c == ESC_CHAR)
            {
                escapeNext = true;
            }
        }
        else
        {
            escapeNext = false;
        }

        ++currentPos;
    }

    // throw ParsingError if str ended without closing STRING_CHAR
    if(isString)
    {
        throw ParsingError(std::string(1, STRING_CHAR), "string end");
    }

    return currentPos;
}


size_t sklxprs::find_plain_end(const std::string& str, size_t plainStart,
                               const std::string& plainDelim)
{
    const char STRING_CHAR = '"';
    const char GROUP_START_CHAR = '(';
    const char GROUP_END_CHAR = ')';

    // check that str isn't all whitespace
    size_t currentPos = str.find_first_not_of(WHITESPACE, plainStart);
    if(currentPos == std::string::npos)
    {
        throw ParsingError("plain object", "string end");
    }

    while(currentPos < str.size())
    {
        const char c = str[currentPos];

        if(plainDelim.find(c) != std::string::npos || WHITESPACE.find(c) != std::string::npos)
        {
            break;
        }
        else if(c == STRING_CHAR)
        {
            currentPos = find_string_end(str, currentPos);
            continue;
        }
        else if(c == GROUP_START_CHAR)
        {
            currentPos = find_block_end(str, currentPos, GROUP_START_CHAR, GROUP_END_CHAR);
            continue;
        }

        ++currentPos;
    }

    // check that we got either delim or whitespace (if delim is required)
    if(currentPos >= str.size() && !plainDelim.empty())
    {
        std::string delimList;
        for(unsigned int i = 0; i < plainDelim.size(); ++i)
        {
            if(i > 0)
            {
                delimList += " or ";
            }
            delimList += plainDelim[i];
        }
        throw ParsingError(delimList, "string end");
    }

    return currentPos;
}


size_t sklxprs::find_group_end(const std::string& str, size_t groupStart)
{
    return find_block_end(str, groupStart, '(', ')');
}


size_t sklxprs::find_map_end(const std::string& str, size_t mapStart)
{
    return find_block_end(str, mapStart, '{', '}');
}


size_t sklxprs::find_list_end(const std::string& str, size_t listStart)
{
    return find_block_end(str, listStart, '[', ']');
}


size_t sklxprs::find_block_end(const std::string& str, size_t blockStart, char start, char end)
{
    const char STRING_CHAR = '"';
    unsigned int blockDepth = 1;

    // check for start char
    size_t currentPos = str.find_first_not_of(WHITESPACE, blockStart);
    if(currentPos == std::string::npos || str[currentPos] != start)
    {
        throw ParsingError(std::string(1, start));
    }
    ++currentPos;

    while(blockDepth != 0 && currentPos < str.size())
    {
        const char c = str[currentPos];

        if(c == STRING_CHAR)
        {
            currentPos = find_string_end(str, currentPos);
            continue;
        }
        else if(c == start)
        {
            ++blockDepth;
        }
        else if(c == end)
        {
            --blockDepth;
        }

        ++currentPos;
    }

    // throw ParsingError if str ended without closing character
    if(blockDepth != 0)
    {
        throw ParsingError(std::string(1, end));
    }

    return currentPos;
}


size_t sklxprs::find_and_test_first_not_of(const std::string& str, size_t searchStart,
                                  const std::string& notThese, const std::string& expect)
{
    size_t firstNot = str.find_first_not_of(notThese, searchStart);
    if(firstNot != std::string::npos)
    {
        if(expect.find(str[firstNot]) == std::string::npos)
        {
            std::string expectList;
            for(unsigned int i = 0; i < expect.size(); ++i)
            {
                if(i > 0)
                {
                    expectList += " or ";
                }
                expectList += expect[i];
            }
            throw ParsingError(expectList, std::string(1, str[firstNot]));
        }
        else
        {
            return firstNot;
        }
    }
    else
    {
        std::string expectList;
        for(unsigned int i = 0; i < expect.size(); ++i)
        {
            if(i > 0)
            {
                expectList += " or ";
            }
            expectList += expect[i];
        }
        throw ParsingError(expectList, "string end");
    }
}
