#include "window.hpp"

#include "project.hpp"

#include <GL/glew.h>
#include <SDL_opengl.h>

#include <iostream>

Window::Window()
    :window(nullptr)
{
    window = SDL_CreateWindow(PROJECT_NAME, 0, 0, 640, 640, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    glContext = SDL_GL_CreateContext(window);
    if(glContext == nullptr)
    {
        std::cerr << "failed to creat gl context: "
                  << SDL_GetError() << std::endl;
        throw std::runtime_error("failed to create gl context");
    }
    //Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        std::cerr << "failed to init glew: "
                  <<  glewGetErrorString(glewError) << std::endl;
        throw std::runtime_error("failed to init glew");
    } //Use Vsync
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
    if(SDL_PollEvent(&e))
    {
        if(e.type == SDL_QUIT)
        {
            shouldStop = true;
        }
        else if(e.type == SDL_WINDOWEVENT)
        {
            if(e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                if(resizeCallback)
                {
                    resizeCallback(e.window.data1, e.window.data2);
                }
            }
        }
        else if(e.type == SDL_KEYDOWN)
        {
            if(e.key.repeat == 0)
            {
                if(keydownCallback)
                {
                    keydownCallback(e.key.keysym.sym);
                }
            }
        }
        else if(e.type == SDL_KEYUP)
        {
            if(e.key.repeat == 0)
            {
                if(keyupCallback)
                {
                    keyupCallback(e.key.keysym.sym);
                }
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
