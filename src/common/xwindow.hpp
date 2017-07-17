#ifndef XWINDOW_HPP
#define XWINDOW_HPP


#include <memory>
#include <functional>


class XWindow
{
public:

    // If not useRoot creates new window else uses virtual root window
    XWindow(bool useRoot = false);

    // Uses window with wid as root
    XWindow(int wid);

    ~XWindow();

    void display();
    bool handle_events();

    void set_resize_callback(std::function<void(unsigned int, unsigned int)> callbk);

private:

    void init_display();

    void create_window();
    void create_gl_context();

    void get_new_visual();
    void get_visual_from_window();
    void set_window_name(const char* s);

    std::function<void(unsigned int, unsigned int)> resizeCallback;

    bool ownsWindow;
    struct xvar;
    std::unique_ptr<xvar> var;

    inline xvar& operator->()
    {
        return *var;
    }

    inline const xvar& operator->() const
    {
        return *var;
    }
};

#endif
