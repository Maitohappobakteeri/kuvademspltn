#ifndef STRKILXPRS_HPP
#define STRKILXPRS_HPP


#include "object.hpp"
#include "list.hpp"
#include "map.hpp"
#include "string.hpp"
#include "plain.hpp"
#include "error.hpp"


namespace sklxprs
{
    std::string read_full_file(const std::string& path);

    Object* read_object_from_file(const std::string& path);
    void write_object_to_file(const std::string& path, const Object& object);
}


#endif
