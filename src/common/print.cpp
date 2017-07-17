#include "print.hpp"


namespace
{
    unsigned int statusLength = 0;
    bool isStatus = false;
    bool isEnabled = true;
}


void clear_status()
{
    if(isStatus)
    {
        for(unsigned int i=0; i < statusLength; ++i)
        {
            std::cout << " ";
        }
        std::cout << "\r";
    }
    isStatus = false;
}


void set_status(unsigned int l)
{
    isStatus = true;
    statusLength = l;
}


void enable_print()
{
    isEnabled = true;
}


void disable_print()
{
    isEnabled = false;
}


bool is_print_enabled()
{
    return isEnabled;
}
