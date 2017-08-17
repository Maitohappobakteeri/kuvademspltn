#ifndef DEMO_SIGNAALI_SIGNAALIDEMO_HPP
#define DEMO_SIGNAALI_SIGNAALIDEMO_HPP


#include "demo.hpp"

#include <pulse/simple.h>

#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>


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

    pa_simple* inputStream;
    pa_sample_spec sampleSpec;

    std::mutex pointLock;
    std::queue<float> pointHeights;

    std::atomic<bool> shouldStop;
    std::thread inputThread;

    DynamicBox paramTextBox;
    float advanceSpeed;
    float pointSize;
    bool useLines;

    glm::vec2 lastPointPosition;
};

#endif
