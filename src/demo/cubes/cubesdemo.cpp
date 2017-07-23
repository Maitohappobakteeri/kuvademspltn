#include "cubesdemo.hpp"


#include "print.hpp"

#include <iostream>
#include <thread>
#include <chrono>


namespace
{
    void short_sleep()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


CubesDemo::CubesDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command)
{

}


CubesDemo::~CubesDemo()
{

}


bool CubesDemo::init()
{
    if(!Demo::init()) return false;

    return true;
}


void CubesDemo::cleanup()
{
    Demo::cleanup();
}


bool CubesDemo::update(float step)
{
    return false;
}


void CubesDemo::render()
{
    renderer->clear_screen();
    renderer->render_rectangle({0.4,0,0}, {0,0}, {1,1}, 0);
}
