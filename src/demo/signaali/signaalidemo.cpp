#include "signaalidemo.hpp"


#include "print.hpp"
#include "rng.hpp"


namespace
{
    const unsigned int SIGNALTEXTURE_SIZE = 2048;

    const float DEFAULT_ADVANCE_SPEED = -0.0008f;
    const float ADVANCE_SPEED_INCREMENT = 0.0001f;

    const float DEFAULT_POINT_SIZE = 0.0175f;
    const float POINT_SIZE_INCREMENT = 0.0025f;
}


SignaaliDemo::SignaaliDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command), signalTextureScale(1,1), shouldStop(false),
     advanceSpeed(DEFAULT_ADVANCE_SPEED), pointSize(DEFAULT_POINT_SIZE), useLines(false)
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
    float advance = this->advanceSpeed / signalTextureScale.y;
    float pointSize = this->pointSize / signalTextureScale.y;

    renderer->set_render_target(*signalFramebuffer);
    renderer->render_rectangle({0.6,0.6,0.6}, {0,0}, {1,1}, 0);
    pointLock.lock();
    renderer->render_texture(signalTexture2.get(), {advance * pointHeights.size(),0}, {1,1}, 0);
    lastPointPosition.x += advance * pointHeights.size();
    while(pointHeights.size() != 0)
    {
        glm::vec2 newPointPosition = glm::vec2(0.9f / signalTextureScale.x
                                               + ((pointHeights.size() - 1)*advance),
                                               pointHeights.front() / signalTextureScale.y);
        if(useLines)
        {
            renderer->render_line({1,0,0}, lastPointPosition, newPointPosition);
        }
        else
        {
            renderer->render_texture(pointTexture.get(), newPointPosition,
                                     {pointSize, pointSize}, 0);
        }
        lastPointPosition = newPointPosition;

        pointHeights.pop();
    }
    pointLock.unlock();

    renderer->set_render_target(*signalFramebuffer2);
    renderer->render_texture(signalTexture.get(), {0,0}, {1,1}, 0);

    renderer->set_render_target_screen();
    renderer->clear_screen();
    renderer->render_texture(signalTexture.get(), {0,0}, renderer->get_view_scale()
                                                         * signalTextureScale, 0);

    if(renderDemoInfo)
    {
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

        renderer->render_string_line(font.get(),
                                     std::wstring(L"Use lines: ") + std::to_wstring(useLines),
                                     {1,1,0.8f},
                                     paramTextBox.box_position({0, 1.0f - lineHeight
                                                                - lineHeight*2*2}),
                                     paramTextBox.box_scale({1.0f, lineHeight}), 0,
                                     Renderer::StringAlign::LEFT);
    }
}


void SignaaliDemo::handle_keydown(SDL_Keycode k)
{
    Demo::handle_keydown(k);

    switch(k)
    {
    case SDLK_u:
        advanceSpeed -= ADVANCE_SPEED_INCREMENT;
        break;
    case SDLK_i:
        advanceSpeed += ADVANCE_SPEED_INCREMENT;
        break;
    case SDLK_j:
        pointSize -= POINT_SIZE_INCREMENT;
        break;
    case SDLK_k:
        pointSize += POINT_SIZE_INCREMENT;
        break;
    case SDLK_l:
        useLines = !useLines;
        break;
    }
}


void SignaaliDemo::handle_resize(unsigned int w, unsigned int h)
{
    Demo::handle_resize(w, h);

    signalTextureScale.x = SIGNALTEXTURE_SIZE / float(w);
    signalTextureScale.y = SIGNALTEXTURE_SIZE / float(h);
    printstatus(signalTextureScale.x, ":", signalTextureScale.y);
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
