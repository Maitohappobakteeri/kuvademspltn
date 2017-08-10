#ifndef DEMO_ROISKE_ROISKEDEMO_HPP
#define DEMO_ROISKE_ROISKEDEMO_HPP


#include "demo.hpp"
#include "loadsprite.hpp"

#include <string>
#include <memory>


class RoiskeDemo : public Demo
{
public:

    RoiskeDemo(const Demo::Args& args, const std::wstring& command);
    ~RoiskeDemo();

protected:

    virtual bool init() override;
    virtual void cleanup() override;

    virtual bool update(float step) override;
    virtual void render() override;

    virtual void handle_resize(unsigned int w, unsigned int h) override;

private:

    std::unique_ptr<Texture> splatterTexture;
    std::unique_ptr<Framebuffer> splatterFramebuffer;

    SpriteModel splatterModel;
    SpriteGroup* splatterSpriteGroup;
    Sprite* splatterSprite;

    glm::vec2 splatterScale;
};

#endif
