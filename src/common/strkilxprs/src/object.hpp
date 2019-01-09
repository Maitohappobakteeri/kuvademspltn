#ifndef STRKILXPRS_OBJECT_HPP
#define STRKILXPRS_OBJECT_HPP


#include <string>


namespace sklxprs
{
    enum StringStyles
    {
        TIGHT,
        SPACED,
        LINES,
        MORE_LINES,
        PREFERRED
    };

    // Base class for different objects to be parsed
    class Object
    {
    protected:
        Object();
    public:
        virtual ~Object();

        operator std::string() const;
        virtual std::string to_string(StringStyles style=PREFERRED) const = 0;

        virtual StringStyles get_preferred_string_style() const;

        enum Type
        {
            MAP,
            LIST,
            STRING,
            PLAIN,
            OTHER
        };

        virtual Type get_type() const = 0;
    };

    // Matches str with object pattern and creates an object of that type from str
    //   Matches Map if first non-whitespace char is '{'
    //   Matches List if first non-whitespace char is '['
    //   Matches String if first non-whitespace char is '"'
    //   Matches Plain if no other object is matched
    //   Throws ParsingError if str has errors
    Object* create_object(const std::string& str);

}


#endif
