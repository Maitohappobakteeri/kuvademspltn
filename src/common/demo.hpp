#ifndef DEMO_HPP
#define DEMO_HPP


#include "project.hpp"

#include "window.hpp"
#include "xwindow.hpp"
#include "renderer/renderer.hpp"
#include "renderer/dynamicbox.hpp"


class FrequencyCounter
{
public:
    FrequencyCounter() :counter(0), lastUpdateTime(0) {}

    void update(unsigned int newTime,  unsigned int& freq)
    {
        ++counter;
        if(newTime - lastUpdateTime >= 1000)
        {
            freq = counter;
            counter = 0;
            lastUpdateTime = newTime;
        }
    }

private:
    unsigned int counter;
    unsigned int lastUpdateTime;
};



class Demo
{
public:

    Demo(bool useXWindow=false);
    ~Demo();

    int run();

protected:

    virtual bool update(float step) = 0;
    virtual void render() = 0;

    virtual void handle_keydown(SDL_Keycode k);
    virtual void handle_keyup(SDL_Keycode k);

    bool usingXWindow;
    union
    {
        Window* window;
        XWindow* xwindow;
    } window;

    Renderer* renderer;
    std::shared_ptr<Font> font;

    // frequencies (runs/second)
    FrequencyCounter renderFreqCounter;
    unsigned int renderFreq;
    FrequencyCounter updateFreqCounter;
    unsigned int updateFreq;

    // runtime settings
    unsigned int updateRate; //updates in second
    bool renderDemoInfo;

    DynamicBox infoTextBox;

private:

    bool update_demo();

    bool initialize_rendering(bool useXWindow);
    void cleanup_rendering();

    void render_info();
};

#endif
