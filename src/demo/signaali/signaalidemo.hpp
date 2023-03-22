#ifndef DEMO_SIGNAALI_SIGNAALIDEMO_HPP
#define DEMO_SIGNAALI_SIGNAALIDEMO_HPP


#include "demo.hpp"

#include <string>
#include <memory>
#include <queue>


class SignaaliDemo : public Demo
{
public:

    SignaaliDemo(const Demo::Args& args, const std::wstring& command);
    ~SignaaliDemo();

protected:

    virtual bool init() override;
    virtual void cleanup() override;

    virtual bool update(float step) override;
    virtual void render() override;

    virtual void handle_keydown(SDL_Keycode k) override;
    virtual void handle_resize(unsigned int w, unsigned int h) override;

private:

    void handle_signal_input();

    std::shared_ptr<Texture> pointTexture;

    std::unique_ptr<Texture> signalTexture;
    std::unique_ptr<Framebuffer> signalFramebuffer;
    std::unique_ptr<Texture> signalTexture2;
    std::unique_ptr<Framebuffer> signalFramebuffer2;

    glm::vec2 signalTextureScale;

    std::queue<float> pointHeights;

    DynamicBox paramTextBox;
    float advanceSpeed;
    float pointSize;
    bool useLines;
    float time;

    glm::vec2 lastPointPosition;
};

#endif
