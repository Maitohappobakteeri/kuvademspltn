#include "signaalidemo.hpp"


#include "print.hpp"
#include "rng.hpp"


namespace
{
    const unsigned int SIGNALTEXTURE_SIZE = 2048;
}


SignaaliDemo::SignaaliDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command), shouldStop(false)
{
    // renderDemoInfo = false;
}


SignaaliDemo::~SignaaliDemo()
{

}


bool SignaaliDemo::init()
{
    if(!Demo::init()) return false;

    signalTexture.reset(new Texture(Texture::create_empty(SIGNALTEXTURE_SIZE,
                                                          SIGNALTEXTURE_SIZE)));
    signalFramebuffer.reset(new Framebuffer(*signalTexture));

    signalTexture2.reset(new Texture(Texture::create_empty(SIGNALTEXTURE_SIZE,
                                                          SIGNALTEXTURE_SIZE)));
    signalFramebuffer2.reset(new Framebuffer(*signalTexture2));

    pointTexture = renderer->load_texture("res/signaali/point.png");

    renderer->set_render_target(*signalFramebuffer);
    renderer->render_rectangle({0.6,0.6,0.6}, {0,0}, {1,1}, 0);
    renderer->set_render_target(*signalFramebuffer2);
    renderer->render_rectangle({0.6,0.6,0.6}, {0,0}, {1,1}, 0);

    sampleSpec.format = PA_SAMPLE_FLOAT32NE;
    sampleSpec.channels = 2;
    sampleSpec.rate = 44100;
    inputStream = pa_simple_new(nullptr, PROJECT_NAME, PA_STREAM_RECORD, nullptr,
                                "Signal in", &sampleSpec, nullptr, nullptr, nullptr);

    inputThread = std::thread([this](){this->handle_signal_input();});

    // SDL_GL_SetSwapInterval(0);

    return true;
}


void SignaaliDemo::cleanup()
{
    shouldStop = true;
    inputThread.join();

    pa_simple_free(inputStream);

    signalFramebuffer.reset();
    signalTexture.reset();

    signalFramebuffer2.reset();
    signalTexture2.reset();

    pointTexture.reset();

    Demo::cleanup();
}


bool SignaaliDemo::update(float step)
{
    return false;
}


void SignaaliDemo::render()
{
    const float advance = -0.0008f;
    const float pointSize = 0.0125f;

    renderer->set_render_target(*signalFramebuffer);
    renderer->render_rectangle({0.6,0.6,0.6}, {0,0}, {1,1}, 0);
    pointLock.lock();
    renderer->render_texture(signalTexture2.get(), {advance * pointHeights.size(),0}, {1,1}, 0);
    while(pointHeights.size() != 0)
    {
        renderer->render_texture(pointTexture.get(), {0.9f + ((pointHeights.size() - 1)*advance),
                                                      pointHeights.front()},
                                                      {pointSize, pointSize}, 0);
        pointHeights.pop();
    }
    pointLock.unlock();

    renderer->set_render_target(*signalFramebuffer2);
    renderer->render_texture(signalTexture.get(), {0,0}, {1,1}, 0);

    renderer->set_render_target_screen();
    renderer->clear_screen();
    renderer->render_texture(signalTexture.get(), {0,0}, {1,1}, 0);
}


void SignaaliDemo::handle_resize(unsigned int w, unsigned int h)
{
    Demo::handle_resize(w, h);
}


void SignaaliDemo::handle_signal_input()
{
    while(!shouldStop)
    {
        const unsigned int BUFFER_SIZE = 64 * 2 * 2;
        float buffer[BUFFER_SIZE];
        int errorCode;
        pa_simple_read(inputStream, buffer, sizeof(float) * BUFFER_SIZE, &errorCode);

        const unsigned int pointSize = 3;
        for(unsigned int p = 0; p < BUFFER_SIZE / pointSize; ++p)
        {
            float sum = 0;
            for(unsigned int i = p * pointSize; i < p * pointSize + pointSize; ++i)
            {
                sum += buffer[i];
            }
            sum /= pointSize;

            pointLock.lock();
            pointHeights.push(sum);
            pointLock.unlock();
        }
    }
}
