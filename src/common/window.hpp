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
    void set_mouse_move_callback(std::function<void(float, float, float, float)> callbk);
    void set_mouse_down_callback(std::function<void(float, float, int)> callbk);
    void set_mouse_up_callback(std::function<void(float, float, int)> callbk);

private:

    SDL_Window* window;
    SDL_GLContext glContext;

    unsigned int width;
    unsigned int height;

    std::function<void(unsigned int, unsigned int)> resizeCallback;
    std::function<void(SDL_Keycode)> keydownCallback;
    std::function<void(SDL_Keycode)> keyupCallback;
    std::function<void(float, float, float, float)> mouseMoveCallback;
    std::function<void(float, float, int)> mouseDownCallback;
    std::function<void(float, float, int)> mouseUpCallback;
};

#endif
