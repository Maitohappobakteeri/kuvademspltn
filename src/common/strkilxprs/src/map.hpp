#ifndef STRKILXPRS_MAP_HPP
#define STRKILXPRS_MAP_HPP


#include "object.hpp"

#include <memory>
#include <map>
#include <string>


namespace sklxprs
{

    // Map of pairs of keys and values in format: { keyA = valueA; keyB = valueB;}
    //   Keys consist of VARIABLE_CHARACTERS
    //   Values are Objects
    class Map : public Object
    {
    public:

        Map();
        Map(const std::string& str);
        virtual ~Map();

        unsigned int size() const;
        void clear();


        template <class T>
        void set(const std::string& key, const T& valueObject)
        {
            items[key] = std::unique_ptr<Object>(new T(valueObject));
        }


        Object* get(const std::string& key);
        Object const* get(const std::string& key) const;

        void remove(const std::string& key);

        virtual std::string to_string(StringStyles style=PREFERRED) const;
        virtual StringStyles get_preferred_string_style() const;
        virtual Type get_type() const;

    private:

        std::map<std::string, std::unique_ptr<Object>> items;
    };

}


#endif
