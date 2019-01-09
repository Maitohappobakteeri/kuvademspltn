#include "error.hpp"

using namespace sklxprs;



//
// ParsingError
//


const std::string ParsingError::ERROR_PART1 = "expected ";
const std::string ParsingError::ERROR_PART2 = " got ";


ParsingError::ParsingError(const std::string& expected, const std::string& got)
    :errorMessage()
{
    if(got.empty())
    {
        errorMessage = ERROR_PART1 + expected;
    }
    else
    {
        errorMessage = ERROR_PART1 + expected + ERROR_PART2 + got;
    }
}


ParsingError::~ParsingError()
{

}


const char* ParsingError::what() const noexcept
{
    return errorMessage.c_str();
}



//
// FileError
//


const std::string FileError::ERROR_PART1 = "unable to read file: ";


FileError::FileError(const std::string& path)
    :errorMessage(ERROR_PART1 + path)
{

}


FileError::~FileError()
{

}


const char* FileError::what() const noexcept
{
    return errorMessage.c_str();
}
