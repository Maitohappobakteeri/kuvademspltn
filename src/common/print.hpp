#ifndef PRINT_HPP
#define PRINT_HPP


#include <iostream>
#include <sstream>


void clear_status();
void set_status(unsigned int l);

void enable_print();
void disable_print();
bool is_print_enabled();


template <class P>
void insert_all(std::ostream& ostream, const P& insertThis)
{
    ostream << insertThis;
}


template <class... Args, class P>
void insert_all(std::ostream& ostream, const P& insertThis, Args... args)
{
    ostream << insertThis;
    insert_all(ostream, args...);
}


template <class... Args>
void println(Args... args)
{
    if(!is_print_enabled()) return;

    clear_status();
    insert_all(std::cout, args...);
    std::cout << std::endl;
};


template <class... Args>
void printstatus(Args... args)
{
    if(!is_print_enabled()) return;

    clear_status();
    std::stringstream sstream;
    insert_all(sstream, args...);
    set_status(sstream.tellp());

    std::cout << sstream.rdbuf();
    std::cout << "\r" << std::flush;
};


#endif
