#include "spritedemo.hpp"


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


SpriteDemo::SpriteDemo(const std::string& spriteFilename, const Demo::Args& args,
                       const std::wstring& command)
    :Demo(args, command), spriteFilename(spriteFilename)
{

}


SpriteDemo::~SpriteDemo()
{

}


bool SpriteDemo::init()
{
    if(!Demo::init()) return false;

    spriteModel = load_spritemodel_from_file(renderer, spriteFilename);

    sgroup = new SpriteGroup(renderer);
    sgroup->create_sprite(create_sprite(spriteModel));

    return true;
}


void SpriteDemo::cleanup()
{
    delete sgroup;

    Demo::cleanup();
}


bool SpriteDemo::update(float step)
{
    return false;
}


void SpriteDemo::render()
{
    renderer->clear();
    renderer->render_spritegroup(*sgroup);
}
