#ifndef STRKILXPRS_LIST_HPP
#define STRKILXPRS_LIST_HPP


#include "object.hpp"

#include <memory>
#include <vector>
#include <string>


namespace sklxprs
{

    // List that begins with '[', has any number of ',' separated items and ends with ']'
    class List : public Object
    {
    public:

        List();
        List(const std::string& str);
        virtual ~List();

        unsigned int size() const;
        void clear();


        template <class T>
        void append(const T& obj)
        {
            items.emplace_back(new T(obj));
        }


        template <class T>
        void insert(unsigned int pos, const T& obj)
        {
            items.insert(items.begin() + pos, new T(obj));
        }


        Object* get(unsigned int i);
        Object const* get(unsigned int i) const;

        void remove(unsigned int i);
        void move(unsigned int from, unsigned int to);

        virtual std::string to_string(StringStyles style=PREFERRED) const;
        virtual StringStyles get_preferred_string_style() const;
        virtual Type get_type() const;

    private:

        std::vector<std::unique_ptr<Object>> items;
    };

}


#endif
