
#include "project.hpp"
#include "print.hpp"

#include "window.hpp"
#include "renderer/renderer.hpp"
#include "renderer/font.hpp"

#include <chrono>
#include <thread>


int main()
{
    println(PROJECT_NAME, " - texttest");

    // initialize rendering
    SDL_Init(SDL_INIT_VIDEO);
    Window* window = new Window();
    Renderer* renderer = new Renderer();
    window->set_resize_callback([renderer](unsigned int w, unsigned int h)
                                {renderer->handle_resize(w,h);});

    // load font
    std::shared_ptr<Font> font = renderer->load_font("res/font/Inconsolata-Regular.ttf", 64);

    // test string
    std::wstring wstr(L"The sequel adds new standard, sub and special weapons to the game,"
                      L" including dual-wield pistols, that allow the player to perform dodge"
                      L" rolls, and jetpacks.\n\n...");

    // line dimensions
    const float lineWidth = 1.0f;
    const float lineHeight = 0.05f;
    Text text(font.get(), wstr, lineHeight, {0, 0.6f}, {lineWidth, lineHeight}, 0);

    // render text test
    while(!window->handle_events())
    {
        renderer->clear_screen();

        renderer->render_string_box(font.get(),
                                     L"XxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxX",
                                     {1.0f, 0.1f, 0.0f, 1.0f},
                                     {0,0}, {1, 1}, 0,
                                     Renderer::StringAlign::BOTTOM_RIGHT);

        renderer->render_string_box(font.get(),
                                     L".........................................................",
                                     {0.7f, 1.0f, 0.0f, 1.0f},
                                     {0,0.75f+lineHeight*2*2},
                                     {lineWidth, lineHeight}, 0);

        renderer->render_string_line(font.get(),
                                    L".........................................................",
                                    {0.7f, 1.0f, 0.0f, 1.0f},
                                    {0,0.75f+lineHeight*2},
                                    {lineWidth, lineHeight}, 0,
                                    Renderer::StringAlign::RIGHT);
        // render text
        renderer->render_text(text);

        renderer->render_string_line(font.get(),
                                     L".........................................................",
                                     {0.7f, 1.0f, 0.0f, 1.0f},
                                     {0,-0.75f},
                                     {lineWidth, lineHeight}, 0);

        window->display();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // cleanup
    font.reset();
    delete renderer;
    delete window;
    SDL_Quit();
}
