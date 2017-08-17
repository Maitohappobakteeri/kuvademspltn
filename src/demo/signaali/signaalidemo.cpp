#include "signaalidemo.hpp"


#include "print.hpp"
#include "rng.hpp"


namespace
{
    const unsigned int SIGNALTEXTURE_SIZE = 2048;

    const float DEFAULT_ADVANCE_SPEED = -0.0008f;
    const float ADVANCE_SPEED_INCREMENT = 0.00005f;

    const float DEFAULT_POINT_SIZE = 0.0125f;
    const float POINT_SIZE_INCREMENT = 0.0025f;
}


SignaaliDemo::SignaaliDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command), signalTextureScale(1,1), shouldStop(false),
     advanceSpeed(DEFAULT_ADVANCE_SPEED), pointSize(DEFAULT_POINT_SIZE)
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

    paramTextBox.set_scale({-0.05f, 0.95f}, DynamicBox::REL_HEIGHT, DynamicBox::OFF_WIDTH);
    paramTextBox.set_align(DynamicBox::RIGHT);

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
    float advance = this->advanceSpeed / signalTextureScale.x;
    float pointSize = this->pointSize;

    renderer->set_render_target(*signalFramebuffer);
    renderer->render_rectangle({0.6,0.6,0.6}, {0,0}, {1,1}, 0);
    pointLock.lock();
    renderer->render_texture(signalTexture2.get(), {advance * pointHeights.size(),0}, {1,1}, 0);
    while(pointHeights.size() != 0)
    {
        renderer->render_texture(pointTexture.get(), {0.9f + ((pointHeights.size() - 1)*advance),
                                                      pointHeights.front()},
                                                      {pointSize / signalTextureScale.x,
                                                       pointSize}, 0);
        pointHeights.pop();
    }
    pointLock.unlock();

    renderer->set_render_target(*signalFramebuffer2);
    renderer->render_texture(signalTexture.get(), {0,0}, {1,1}, 0);

    renderer->set_render_target_screen();
    renderer->clear_screen();
    renderer->render_texture(signalTexture.get(), {0,0}, signalTextureScale, 0);

    const float lineHeight = 0.04f;

    paramTextBox.update(*renderer);

    renderer->render_string_box(font.get(),
                                std::wstring(L"Advance speed: ") + std::to_wstring(advanceSpeed),
                                {1,1,0.8f},
                                paramTextBox.box_position({0, 1.0f - lineHeight
                                                              - lineHeight*2*0}),
                                paramTextBox.box_scale({1.0f, lineHeight}), 0,
                                Renderer::StringAlign::LEFT);

    renderer->render_string_line(font.get(),
                                 std::wstring(L"Point size: ") + std::to_wstring(pointSize),
                                 {1,1,0.8f},
                                 paramTextBox.box_position({0, 1.0f - lineHeight
                                                               - lineHeight*2*1}),
                                 paramTextBox.box_scale({1.0f, lineHeight}), 0,
                                 Renderer::StringAlign::LEFT);
}


void SignaaliDemo::handle_resize(unsigned int w, unsigned int h)
{
    Demo::handle_resize(w, h);

    glm::vec2 viewScale = renderer->get_view_scale();
    signalTextureScale = viewScale;
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