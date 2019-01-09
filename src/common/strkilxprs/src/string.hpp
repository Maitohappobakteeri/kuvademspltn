#ifndef STRKILXPRS_STRING_HPP
#define STRKILXPRS_STRING_HPP


#include "object.hpp"

#include <string>


namespace sklxprs
{

    // String begins and ends with '"'
    class String : public Object
    {
    public:

        String();
        String(const std::string& str);
        virtual ~String();

        // Create String from unformatted std::string
        static String from_pure(const std::string& pureStr);

        virtual std::string to_string(StringStyles style=PREFERRED) const;
        virtual Type get_type() const;

        // Return unformatted std::string
        std::string get_pure() const;

        static std::string format_string(const std::string& pureStr);
        static std::string parse_string(const std::string& formattedStr);

    private:

        std::string pureString;
    };

}


#endif
