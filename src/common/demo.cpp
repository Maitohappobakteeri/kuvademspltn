#include "demo.hpp"


#include "resid.hpp"
#include "loadsprite.hpp"
#include "print.hpp"
#include "time.hpp"

#include <iostream>
#include <thread>
#include <chrono>


namespace
{
    void short_sleep()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }


    boost::program_options::options_description init_option_desc()
    {
        namespace opt = boost::program_options;
        opt::options_description desc("Allowed options");
        desc.add_options()
            ("help", "show this")
            ("root", "use virtual root window")
            ("window-id", opt::value<std::string>(), "use window with ID as root window")
        ;
        return desc;
    }

    boost::program_options::options_description get_option_desc()
    {
        namespace opt = boost::program_options;
        static opt::options_description desc = init_option_desc();
        return desc;
    }
}


Demo::Demo(const Demo::Args& args, const std::wstring& command)
     :window{nullptr}, renderer(nullptr),
      updateRate(60), renderDemoInfo(true), command(command),
      renderFreq(0), updateFreq(0), args(args),
      updateTimeCounter(0), lastUpdateTime(0)
{

}


Demo::~Demo()
{

}


int Demo::run()
{
    init();

    // main loop
    println("starting main loop");
    lastUpdateTime = time_ms();
    bool gameRunning = true;
    while(gameRunning)
    {
        gameRunning = !update_demo();
        if(!gameRunning) continue;

        render();
        if(renderDemoInfo) render_info();

        if(usingXWindow)
        {
            window.xwindow->display();
        }
        else
        {
            window.window->display();
        }

        renderFreqCounter.update(time_ms(), renderFreq);

        // printstatus("render: ", renderFreq, " ", "update: ", updateFreq);
        short_sleep();
    }

    cleanup();

    println("returning from demo");
    return 0;
}


bool Demo::init()
{
    bool useXWindow = args.useRootWindow;
    println("creating window");
    if(!useXWindow)
    {
        initialize_window();
    }
    else
    {
        if(args.rootWindowID != -1)
        {
            initialize_xwindow(args.rootWindowID);
        }
        else
        {
            initialize_xwindow(true);
        }
    }

    println("initializing rendering");
    initialize_rendering();

    set_window_callbacks();

    return true;
}


void Demo::cleanup()
{
    println("releasing memory");
    cleanup_rendering();
}


Demo::Args Demo::create_args(const boost::program_options::variables_map& vm)
{
    Args args;

    if(vm.count("help"))
    {
        args.printHelp = true;
        args.shouldRun = false;
    }

    if(vm.count("root") || (vm.count("window-id")))
    {
        args.useRootWindow = true;
        args.disablePrint = true;

        if(vm.count("window-id"))
        {
            args.rootWindowID = std::stoi(vm["window-id"].as<std::string>(), nullptr, 16);
        }
    }

    return args;
}


boost::program_options::options_description Demo::options()
{
    return get_option_desc();
}


bool Demo::initialize_window()
{
    usingXWindow = false;

    SDL_Init(SDL_INIT_VIDEO);
    window.window = new Window();
    return true;
}


bool Demo::initialize_xwindow(bool useRoot)
{
    usingXWindow = true;
    window.xwindow = new XWindow(useRoot);
    return true;
}


bool Demo::initialize_xwindow(int rootwid)
{
    usingXWindow = true;
    window.xwindow = new XWindow(rootwid);
    return true;
}


void Demo::set_window_callbacks()
{
    if(usingXWindow)
    {
        window.xwindow->set_resize_callback(
                            [this](unsigned int w, unsigned int h){handle_resize(w, h);}
                        );
    }
    else
    {
        window.window->set_resize_callback(
                            [this](unsigned int w, unsigned int h){handle_resize(w, h);}
                        );
        window.window->set_keydown_callback(
                            [this](SDL_Keycode k){handle_keydown(k);}
                        );
        window.window->set_keyup_callback(
                            [this](SDL_Keycode k){handle_keyup(k);}
                        );
        window.window->set_mouse_move_callback(
                            [this](float x, float y, float mx, float my)
                            {handle_mouse_move(x, y, mx, my);}
                        );
        window.window->set_mouse_down_callback(
                            [this](float x, float y, int b)
                            {handle_mouse_down(x, y, b);}
                        );
        window.window->set_mouse_up_callback(
                            [this](float x, float y, int b)
                            {handle_mouse_up(x, y, b);}
                        );
    }
}


bool Demo::initialize_rendering()
{
    try
    {
        renderer = new Renderer();
    }
    catch(std::runtime_error& e)
    {
        return false;
    }

    font = renderer->load_font(RES_COMMON_FONT_INCONSOLATA_REGULAR, 32);

    infoTextBox.set_scale({-0.05f, 0.95f}, Relative::HEIGHT, Offset::WIDTH);
    infoTextBox.set_align(Align::LEFT);

    return true;
}


void Demo::cleanup_rendering()
{
    font.reset();
    delete renderer;

    if(usingXWindow)
    {
        delete window.xwindow;
    }
    else
    {
        delete window.window;
    }

    SDL_Quit();
}


void Demo::render_info()
{
    const float lineHeight = 0.04f;

    infoTextBox.update(*renderer);

    renderer->render_string_box(font.get(),
                                command,
                                {1,1,0.8f},
                                infoTextBox.box_position({0, 1.0f - lineHeight - lineHeight*2*0}),
                                infoTextBox.box_scale({1.0f, lineHeight}), 0,
                                Align::RIGHT);

    renderer->render_string_line(font.get(),
                                 std::wstring(L"Update: ") + std::to_wstring(updateFreq),
                                 {1,1,0.8f},
                                 infoTextBox.box_position({0, 1.0f - lineHeight - lineHeight*2*1}),
                                 infoTextBox.box_scale({1.0f, lineHeight}), 0,
                                 Align::RIGHT);

    renderer->render_string_line(font.get(),
                                 std::wstring(L"Render: ") + std::to_wstring(renderFreq),
                                 {1,1,0.8f},
                                 infoTextBox.box_position({0, 1.0f - lineHeight - lineHeight*2*2}),
                                 infoTextBox.box_scale({1.0f, lineHeight}), 0,
                                 Align::RIGHT);
}


bool Demo::update_demo()
{
    double timeBetweenUpdates = 1.0 / float(updateRate);

    unsigned int newTime = time_ms();
    updateTimeCounter += double(newTime - lastUpdateTime) / 1000.0;
    lastUpdateTime = newTime;

    unsigned int updateCounter = 0;
    while(updateTimeCounter >= timeBetweenUpdates)
    {
        if(updateCounter >= 5)
        {
            updateTimeCounter = 0;
            break;
        }
        ++updateCounter;

        if(usingXWindow)
        {
            if(window.xwindow->handle_events())
            {
                return true;
            }
        }
        else
        {
            if(window.window->handle_events())
            {
                return true;
            }
        }

        updateTimeCounter -= timeBetweenUpdates;

        if(update(timeBetweenUpdates))
        {
            return true;
        }

        updateFreqCounter.update(newTime, updateFreq);
    }

    return false;
}


void Demo::handle_keydown(SDL_Keycode k)
{
    switch(k)
    {
        case SDLK_t:
            renderDemoInfo = !renderDemoInfo;
            break;
    }
}


void Demo::handle_keyup(SDL_Keycode k)
{

}


void Demo::handle_mouse_move(float positionX, float positionY, float moveX, float moveY)
{

}


void Demo::handle_mouse_down(float positionX, float positionY, int button)
{

}


void Demo::handle_mouse_up(float positionX, float positionY, int button)
{

}


void Demo::handle_resize(unsigned int w, unsigned int h)
{
    renderer->handle_resize(w, h);
}


std::pair<std::string, std::string> is_wid_option(const std::string& s)
{
    if(s.find("-window-id") == 0)
    {
        return std::make_pair("window-id", std::string());
    }
    return std::make_pair(std::string(), std::string());
}


std::wstring join_command(int argc, char* argv[])
{
    std::wstring command;
    if(argc > 0)
    {
        std::string arg(argv[0]);
        command.append(arg.begin(), arg.end());
    }
    for(int i = 1; i < argc; ++i)
    {
        command += L" ";
        std::string arg(argv[i]);
        command.append(arg.begin(), arg.end());
    }
    return command;
}
