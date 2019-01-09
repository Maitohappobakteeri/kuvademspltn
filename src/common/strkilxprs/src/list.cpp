#include "list.hpp"


#include "plain.hpp"
#include "parse.hpp"
#include "error.hpp"


using namespace sklxprs;


List::List()
{

}


List::List(const std::string& str)
{
    // check for [
    size_t currentPos = find_and_test_first_not_of(str, 0, WHITESPACE, "[");
    ++currentPos;

    currentPos = str.find_first_not_of(WHITESPACE, currentPos);
    while(str[currentPos] != ']')
    {
        // item start
        currentPos = str.find_first_not_of(WHITESPACE, currentPos);
        if(currentPos == std::string::npos)
        {
            throw ParsingError("list item", "string end");
        }
        size_t itemStartPos = currentPos;

        // item end
        currentPos = find_object_end(str, currentPos, ",]");

        // take item substring
        std::string itemStr = str.substr(itemStartPos, currentPos - itemStartPos);
        if(itemStr.size() == 0)
        {
            throw ParsingError("list item", std::string(1, str[currentPos]));
        }

        // create new object
        items.emplace_back(create_object(itemStr));

        // check if list ends or continues
        currentPos = str.find_first_not_of(WHITESPACE, currentPos);
        if(str[currentPos] == ',')
        {
            ++currentPos;
        }
        else if(str[currentPos] != ']')
        {
            throw ParsingError(", or ]", std::string(1, str[currentPos]));
        }
    }
}


List::~List()
{

}


unsigned int List::size() const
{
    return items.size();
}


void List::clear()
{
    items.clear();
}


Object* List::get(unsigned int i)
{
    if(i >= size())
    {
        return nullptr;
    }
    else
    {
        return items[i].get();
    }
}


Object const* List::get(unsigned int i) const
{
    if(i >= size())
    {
        return nullptr;
    }
    else
    {
        return items[i].get();
    }
}


void List::remove(unsigned int i)
{
    if(i >= size())
    {
        throw std::out_of_range("List index out of range");
    }

    items.erase(items.begin() + i);
}


void List::move(unsigned int from, unsigned int to)
{
    if(from >= size() || to >= size())
    {
        throw std::out_of_range("List index out of range");
    }

    std::unique_ptr<Object> tmpPtr = std::move(items[from]);
    items.erase(items.begin() + from);
    items.insert(items.begin() + to, std::move(tmpPtr));
}


std::string List::to_string(StringStyles style) const
{
    if(style == PREFERRED)
    {
        style = get_preferred_string_style();
    }

    if(style == TIGHT)
    {
        if(size() == 0)
        {
            return std::string("[]");
        }
        else
        {
            std::string str = std::string("[") + items[0]->to_string();
            for(unsigned int i = 1; i < size(); ++i)
            {
                str += ",";
                str += items[i]->to_string();
            }
            str += "]";
            return str;
        }
    }
    else if(style == SPACED)
    {
        if(size() == 0)
        {
            return std::string("[ ]");
        }
        else
        {
            std::string str = std::string("[ ") + items[0]->to_string();
            for(unsigned int i = 1; i < size(); ++i)
            {
                str += ", ";
                str += items[i]->to_string();
            }
            str += " ]";
            return str;
        }
    }
    else if(style == LINES || style == MORE_LINES)
    {
        if(size() == 0)
        {
            return std::string("\n\n");
        }
        else
        {
            std::string str = std::string("[\n");
            for(unsigned int i = 0; i < size(); ++i)
            {
                if(i > 0)
                {
                    str += ",\n";
                }
                std::string itemStr = items[i]->to_string();

                str += INDENTATION;
                size_t newlinePos = itemStr.find("\n");
                str += itemStr.substr(0, newlinePos);

                while(newlinePos != std::string::npos)
                {
                    size_t lineStart = newlinePos + 1;
                    newlinePos = itemStr.find("\n", lineStart);

                    str += "\n";
                    str += INDENTATION;
                    str += itemStr.substr(lineStart, newlinePos - lineStart);
                }
            }

            str += "\n]";
            return str;
        }
    }
    
    throw std::runtime_error("this should never be reached");
}


StringStyles List::get_preferred_string_style() const
{
    for(auto& item : items)
    {
        if(item->get_preferred_string_style() == LINES)
        {
            return LINES;
        }
    }
    return SPACED;
}


Object::Type List::get_type() const
{
    return Type::LIST;
}
