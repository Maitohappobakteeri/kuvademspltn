#ifndef STRKILXPRS_ERROR_HPP
#define STRKILXPRS_ERROR_HPP


#include <exception>
#include <string>


namespace sklxprs
{
    class ParsingError : public std::exception
    {
    public:

        ParsingError(const std::string& expected, const std::string& got="");
        virtual ~ParsingError();

        virtual const char* what() const noexcept;

    private:

        static const std::string ERROR_PART1;
        static const std::string ERROR_PART2;

        std::string errorMessage;
    };


    class FileError : public std::exception
    {
    public:

        FileError(const std::string& path);
        virtual ~FileError();

        virtual const char* what() const noexcept;

    private:

        static const std::string ERROR_PART1;

        std::string errorMessage;
    };
}


#endif
