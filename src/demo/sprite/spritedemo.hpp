#ifndef DEMO_SPRITE_SPRITEDEMO_HPP
#define DEMO_SPRITE_SPRITEDEMO_HPP


#include "demo.hpp"
#include "loadsprite.hpp"

#include <string>


class SpriteDemo : public Demo
{
public:

    SpriteDemo(const std::string& spriteFilename, const Demo::Args& args,
               const std::wstring& command);
    ~SpriteDemo();

protected:

    virtual bool init() override;
    virtual void cleanup() override;

    virtual bool update(float step) override;
    virtual void render() override;

private:

    std::string spriteFilename;
    SpriteModel spriteModel;
};

#endif
