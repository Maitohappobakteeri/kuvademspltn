#include "roiskedemo.hpp"


#include "print.hpp"
#include "rng.hpp"
#include "loadsprite.hpp"


namespace
{
    const unsigned int SPLATTERTEXTURE_SIZE = 2080;
    const float RESET_TIME = 0.3f;
}



RoiskeDemo::RoiskeDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command), resetCounter(0), rotationVelocity(0), velocity()
{
    renderDemoInfo = false;
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

    splatterSpriteGroup = new SpriteGroup(renderer);
    splatterSpriteGroup->set_scale({30.0f, 30.0f});

    splatterModel = load_spritemodel_from_file(renderer, "res/roiske/blood.sprite");
    splatterSprite = splatterSpriteGroup->create_sprite(create_sprite(splatterModel));

    return true;
}


void RoiskeDemo::cleanup()
{
    delete splatterSpriteGroup;

    splatterFramebuffer.reset();
    splatterTexture.reset();

    Demo::cleanup();
}


bool RoiskeDemo::update(float step)
{
    resetCounter += step;
    if(resetCounter > RESET_TIME)
    {
        glm::vec2 splatterScale = renderer->get_view_scale();

        resetCounter -= RESET_TIME;

        splatterSpriteGroup->remove_sprite(splatterSprite);
        splatterSprite = splatterSpriteGroup->create_sprite(create_sprite(splatterModel));

        std::uniform_real_distribution<float> realDist(-1.0f, 1.0f);
        std::uniform_real_distribution<float> realDist2(0.0f, 2.0f);
        splatterSprite->set_position(10.0f * glm::vec2{splatterScale.x * realDist(randGen),
                                                       splatterScale.y * realDist(randGen)});

        float r = realDist(randGen) * 6.3f;
        splatterSprite->set_rotation(r);
        velocity = glm::vec2(std::cos(-r), std::sin(-r)) * realDist2(randGen);
        rotationVelocity = realDist(randGen) * 1.0f;
    }

    splatterSprite->set_position(splatterSprite->get_position() + velocity * step);
    splatterSprite->set_rotation(splatterSprite->get_rotation() + rotationVelocity * step);

    return false;
}


void RoiskeDemo::render()
{
    renderer->set_render_target(*splatterFramebuffer);
    renderer->render_spritegroup(*splatterSpriteGroup);
    renderer->set_render_target_screen();
    renderer->clear();
    renderer->render_texture(splatterTexture.get(), {0,0}, {3, 3}, 0);
}


void RoiskeDemo::handle_resize(unsigned int w, unsigned int h)
{
    Demo::handle_resize(w, h);
}
