#include "window.hpp"


#include "project.hpp"

#include <GL/glew.h>
#include <SDL_opengl.h>

#include <iostream>


Window::Window()
    :window(nullptr), width(640), height(640)
{
    window = SDL_CreateWindow(PROJECT_NAME, 0, 0, width, height, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_ES);

    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr)
    {
        std::cerr << "failed to create gl context: "
                  << SDL_GetError() << std::endl;
        throw std::runtime_error("failed to create gl context");
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        std::cerr << "failed to init glew: "
                  <<  glewGetErrorString(glewError) << std::endl;
        throw std::runtime_error("failed to init glew");
    }

    if(SDL_GL_SetSwapInterval( 1 ) < 0)
    {
        std::cerr << "failed to enable vsync: " << SDL_GetError() << std::endl;
    }
}


Window::~Window()
{
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
}


void Window::display()
{
    SDL_GL_SwapWindow(window);
}


bool Window::handle_events()
{
    bool shouldStop = false;

    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_QUIT:
            {
                shouldStop = true;
                break;
            }
            case SDL_WINDOWEVENT:
            {
                if(e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    if(resizeCallback)
                    {
                        width = e.window.data1;
                        height = e.window.data2;
                        resizeCallback(width, height);
                    }
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                if(e.key.repeat == 0)
                {
                    if(keydownCallback)
                    {
                        keydownCallback(e.key.keysym.sym);
                    }
                }
                break;
            }
            case SDL_KEYUP:
            {
                if(e.key.repeat == 0)
                {
                    if(keyupCallback)
                    {
                        keyupCallback(e.key.keysym.sym);
                    }
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {
                if(mouseMoveCallback)
                {
                    mouseMoveCallback(float(e.motion.x)/float(width),
                                      float(e.motion.y)/float(height),
                                      float(e.motion.xrel)/float(width),
                                      float(e.motion.yrel)/float(height));
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if(mouseDownCallback)
                {
                    int button;
                    switch(e.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            button = 1;
                            break;
                        case SDL_BUTTON_RIGHT:
                            button = 2;
                            break;
                        case SDL_BUTTON_MIDDLE:
                            button = 3;
                            break;
                        case SDL_BUTTON_X1:
                            button = 4;
                            break;
                        case SDL_BUTTON_X2:
                            button = 5;
                            break;
                    }
                    mouseDownCallback(float(e.button.x)/float(width),
                                      float(e.button.y)/float(height),
                                      button);
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if(mouseUpCallback)
                {
                    int button;
                    switch(e.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            button = 1;
                            break;
                        case SDL_BUTTON_RIGHT:
                            button = 2;
                            break;
                        case SDL_BUTTON_MIDDLE:
                            button = 3;
                            break;
                        case SDL_BUTTON_X1:
                            button = 4;
                            break;
                        case SDL_BUTTON_X2:
                            button = 5;
                            break;
                    }
                    mouseUpCallback(float(e.button.x)/float(width),
                                    float(e.button.y)/float(height),
                                    button);
                }
                break;
            }
        }
    }

    return shouldStop;
}


void Window::set_resize_callback(std::function<void(unsigned int, unsigned int)> callbk)
{
    resizeCallback = callbk;

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if(resizeCallback) resizeCallback(w, h);
}


void Window::set_keydown_callback(std::function<void(SDL_Keycode)> callbk)
{
    keydownCallback = callbk;
}


void Window::set_keyup_callback(std::function<void(SDL_Keycode)> callbk)
{
    keyupCallback = callbk;
}


void Window::set_mouse_move_callback(std::function<void(float, float, float, float)> callbk)
{
    mouseMoveCallback = callbk;
}


void Window::set_mouse_down_callback(std::function<void(float, float, int)> callbk)
{
    mouseDownCallback = callbk;
}


void Window::set_mouse_up_callback(std::function<void(float, float, int)> callbk)
{
    mouseUpCallback = callbk;
}
