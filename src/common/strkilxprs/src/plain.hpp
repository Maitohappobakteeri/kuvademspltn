#ifndef STRKILXPRS_PLAIN_HPP
#define STRKILXPRS_PLAIN_HPP


#include "object.hpp"

#include <string>


namespace sklxprs
{

    class Plain : public Object
    {
    public:

        Plain();
        Plain(const std::string& str);
        virtual ~Plain();

        virtual std::string to_string(StringStyles style=PREFERRED) const;
        virtual Type get_type() const;

    private:

        std::string content;
    };

}


#endif
