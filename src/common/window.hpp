#ifndef DESKTOP_WINDOW_HPP
#define DESKTOP_WINDOW_HPP

#include <SDL.h>

#include <functional>

class Window
{
public:

    Window();
    ~Window();

    void display();
    bool handle_events();

    void set_resize_callback(std::function<void(unsigned int, unsigned int)> callbk);
    void set_keydown_callback(std::function<void(SDL_Keycode)> callbk);
    void set_keyup_callback(std::function<void(SDL_Keycode)> callbk);

private:

    SDL_Window* window;
    SDL_GLContext glContext;

    std::function<void(unsigned int, unsigned int)> resizeCallback;
    std::function<void(SDL_Keycode)> keydownCallback;
    std::function<void(SDL_Keycode)> keyupCallback;
};

#endif
