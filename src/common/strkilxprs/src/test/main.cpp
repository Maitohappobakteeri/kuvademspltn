
#include "strkilxprs.hpp"

#include <iostream>
#include <fstream>


namespace prs = sklxprs;


int main()
{
    prs::Map m(prs::read_full_file("testobject.txt"));
    prs::Map m2(m.to_string(prs::StringStyles::MORE_LINES));
    std::cout << m2.to_string() << std::endl;
    std::cout << static_cast<prs::String*>(m2.get("fourNewlines"))->get_pure() << std::endl;
}
