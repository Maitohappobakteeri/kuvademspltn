#ifndef STRKILXPRS_PARSE_HPP
#define STRKILXPRS_PARSE_HPP


#include <string>


namespace sklxprs
{

    // Match first non-whitespace char of str and find matched object end
    size_t find_object_end(const std::string& str, size_t objectStart, const std::string& delim);

    size_t find_plain_end(const std::string& str, size_t plainStart,
                          const std::string& delim);
    size_t find_string_end(const std::string& str, size_t stringStart);

    size_t find_group_end(const std::string& str, size_t groupStart);
    size_t find_map_end(const std::string& str, size_t mapStart);
    size_t find_list_end(const std::string& str, size_t listStart);
    size_t find_block_end(const std::string& str, size_t blockStart, char start, char end);

    // Find first char not of notThese and test if it is in expect
    //   Throw ParsingError if no char if found or found char isn't in expect
    size_t find_and_test_first_not_of(const std::string& str, size_t searchStart,
                                      const std::string& notThese, const std::string& expect);

    const std::string WHITESPACE = " \n\t";
    const std::string LOWER_LETTERS = "abcdefghijklmnopqrstuvwxyz";
    const std::string UPPER_LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string VARIABLE_CHARACTERS = LOWER_LETTERS + UPPER_LETTERS + "_";

    const std::string INDENTATION = "  "; // TWO SPACES

}


#endif
