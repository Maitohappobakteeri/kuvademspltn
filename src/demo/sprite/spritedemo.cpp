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


SpriteDemo::SpriteDemo(const std::string& spritePath, const Demo::Args& args,
                       const std::wstring& command)
    :Demo(args, command)
{
    spriteModel = read_spritedata_from_file(renderer, spritePath);
    renderer->create_spritegroup()->create_sprite(create_sprite(spriteModel));
}


SpriteDemo::~SpriteDemo()
{

}


bool SpriteDemo::update(float step)
{
    return false;
}


void SpriteDemo::render()
{
    renderer->clear_screen();
    renderer->render_sprites();
}
