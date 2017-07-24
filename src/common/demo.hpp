#ifndef DEMO_HPP
#define DEMO_HPP


#include "project.hpp"

#include "window.hpp"
#include "xwindow.hpp"
#include "renderer/renderer.hpp"
#include "renderer/dynamicbox.hpp"

#include <boost/program_options.hpp>


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

        bool disablePrint = false;

        bool useRootWindow = false;
        int rootWindowID = -1;
    };

    static Args create_args(const boost::program_options::variables_map& vm);
    static boost::program_options::options_description options();

    Demo(const Args& args, const std::wstring& command);
    ~Demo();

    int run();

protected:

    virtual bool init();
    virtual void cleanup();

    virtual bool update(float step) = 0;
    virtual void render() = 0;

    virtual void handle_keydown(SDL_Keycode k);
    virtual void handle_keyup(SDL_Keycode k);
    virtual void handle_mouse_move(float positionX, float positionY, float moveX, float moveY);
    virtual void handle_mouse_down(float positionX, float positionY, int button);
    virtual void handle_mouse_up(float positionX, float positionY, int button);
    virtual void handle_resize(unsigned int w, unsigned int h);

    union
    {
        Window* window;
        XWindow* xwindow;
    } window;

    Renderer* renderer;
    std::shared_ptr<Font> font;

    // runtime settings
    unsigned int updateRate; //updates in second
    bool renderDemoInfo;

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

    // frequencies (runs/second)
    FrequencyCounter renderFreqCounter;
    unsigned int renderFreq;
    FrequencyCounter updateFreqCounter;
    unsigned int updateFreq;

    Args args;
    bool usingXWindow;
};


std::pair<std::string, std::string> is_wid_option(const std::string& s);
std::wstring join_command(int argc, char* argv[]);


#endif
