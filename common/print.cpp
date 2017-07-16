#include "print.hpp"


namespace
{
    unsigned int statusLength = 0;
    bool isStatus = false;
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
