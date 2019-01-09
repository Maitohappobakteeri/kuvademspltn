#include "strkilxprs.hpp"


#include <fstream>


using namespace sklxprs;


std::string sklxprs::read_full_file(const std::string& path)
{
    // open file
    std::ifstream file(path);
    if(!file.good())
    {
        file.close();
        throw FileError(path);
    }

    // read file
    std::string fullFileStr((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    if(file.fail())
    {
        file.close();
        throw FileError(path);
    }

    // close file and create object
    file.close();
    return fullFileStr;
}


Object* sklxprs::read_object_from_file(const std::string& path)
{
    return create_object(read_full_file(path));
}


void sklxprs::write_object_to_file(const std::string& path, const Object& object)
{
    // open file
    std::ofstream file(path, std::ofstream::trunc);
    if(!file.good())
    {
        file.close();
        throw FileError(path);
    }

    std::string objectStr = object.to_string();
    file.write(objectStr.c_str(), objectStr.size());

    file.close();
    if(file.fail())
    {
        throw FileError(path);
    }
}
