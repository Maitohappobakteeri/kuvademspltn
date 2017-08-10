#include "roiskedemo.hpp"


#include "print.hpp"
#include "rng.hpp"
#include "loadsprite.hpp"


namespace
{
    const unsigned int SPLATTERTEXTURE_SIZE = 2080;
}



RoiskeDemo::RoiskeDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command)
{
    // renderDemoInfo = false;
}


RoiskeDemo::~RoiskeDemo()
{

}


bool RoiskeDemo::init()
{
    if(!Demo::init()) return false;

    splatterTexture.reset(new Texture(Texture::create_empty(SPLATTERTEXTURE_SIZE,
                                                            SPLATTERTEXTURE_SIZE)));
    splatterFramebuffer.reset(new Framebuffer(*splatterTexture));

    splatterSpriteGroup = renderer->create_spritegroup();
    splatterSpriteGroup->set_scale({10.0f, 10.0f});
    splatterScale = glm::vec2(1.0f, 1.0f);

    splatterModel = read_spritedata_from_file(renderer, "res/roiske/blood.sprite");
    splatterSprite = splatterSpriteGroup->create_sprite(create_sprite(splatterModel));

    return true;
}


void RoiskeDemo::cleanup()
{
    splatterFramebuffer.reset();
    splatterTexture.reset();

    Demo::cleanup();
}


bool RoiskeDemo::update(float step)
{
    static float r = 0;
    r += step;

    splatterSprite->set_rotation(r);

    return false;
}


void RoiskeDemo::render()
{
    renderer->set_render_target(*splatterFramebuffer);
    renderer->render_sprites();
    renderer->set_render_target_screen();
    renderer->clear_screen();
    renderer->render_texture(splatterTexture.get(), {0,0}, splatterScale, 0);
}


void RoiskeDemo::handle_resize(unsigned int w, unsigned int h)
{
    Demo::handle_resize(w, h);
}
