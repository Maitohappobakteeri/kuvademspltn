#ifndef DEMO_SPRITE_SPRITEDEMO_HPP
#define DEMO_SPRITE_SPRITEDEMO_HPP


#include "demo.hpp"
#include "loadsprite.hpp"

#include <string>


class SpriteDemo : public Demo
{
public:

    SpriteDemo(const std::string& spritePath);
    ~SpriteDemo();

protected:

    virtual bool update(float step) override;
    virtual void render() override;

private:

    SpriteModel spriteModel;
};

#endif
