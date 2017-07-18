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

    struct Args
    {
        bool printHelp = false;
        bool shouldRun = true;
        std::wstring command;

        bool disablePrint = false;

        bool useRootWindow = false;
        int rootWindowID = -1;
    };

    static Args parse_args(int argc, char* argv[]);
    static void print_options();

    Demo(const Args& args);
    ~Demo();

    int run();

protected:

    virtual bool update(float step) = 0;
    virtual void render() = 0;

    virtual void handle_keydown(SDL_Keycode k);
    virtual void handle_keyup(SDL_Keycode k);

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
    bool usingXWindow;

    DynamicBox infoTextBox;
    std::wstring command;

private:

    bool update_demo();

    bool initialize_window();
    bool initialize_xwindow(bool useRoot);
    bool initialize_xwindow(int rootwid);
    void set_window_callbacks();

    bool initialize_rendering();
    void cleanup_rendering();

    void render_info();
};

#endif
