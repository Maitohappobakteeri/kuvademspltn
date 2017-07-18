#include "xwindow.hpp"


#include "project.hpp"

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "vroot.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <cstring>
#include <csignal>


namespace
{
    Window get_root_window(Display* display);
    Window get_virtual_root_window(Display* display);

    Atom get_delete_window_atom(Display* display, Window window);

    int isExtensionSupported(const char *extList, const char *extension);
    int ctxErrorHandler(Display *dpy, XErrorEvent *ev);

    static int fb_attribs[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 16,
        None
    };

    bool terminated = false;
    void termHandler(int signum)
    {
        terminated = true;
    }
};


struct XWindow::xvar
{
    Display* display;
    Window window;

    XVisualInfo* vi;
    GLXFBConfig fbc;
    Colormap cmap;
    GLXContext glc;

    int deleteWindowAtom;
};


XWindow::XWindow(bool useRoot)
    :ownsWindow(!useRoot), var(new xvar())
{
    signal(SIGTERM, termHandler);
    signal(SIGINT, termHandler);

    init_display();

    if(useRoot)
    {
        var->window = get_virtual_root_window(var->display);
        get_visual_from_window();
    }
    else
    {
        create_window();
    }

    set_window_name(PROJECT_NAME);
    create_gl_context();
    var->deleteWindowAtom = get_delete_window_atom(var->display, var->window);
}


XWindow::XWindow(int wid)
    :ownsWindow(false), var(new xvar())
{
    signal(SIGTERM, termHandler);
    signal(SIGINT, termHandler);

    init_display();
    var->window = wid;
    get_visual_from_window();

    set_window_name(PROJECT_NAME);
    create_gl_context();
    var->deleteWindowAtom = get_delete_window_atom(var->display, var->window);
}


XWindow::~XWindow()
{
    glXMakeCurrent(var->display, None, NULL);
    glXDestroyContext(var->display, var->glc);

    if(ownsWindow)
    {
        XDestroyWindow(var->display, var->window);
    }

    XFreeColormap(var->display, var->cmap);
    XCloseDisplay(var->display);
}


void XWindow::init_display()
{
    var->display = XOpenDisplay(getenv("DISPLAY"));
    if(var->display == nullptr)
    {
        std::cerr << "cannot open a x display" << std::endl;
        throw std::runtime_error("cannot open a x display");
    }
}


void XWindow::display()
{
    glXSwapBuffers(var->display, var->window);
}


bool XWindow::handle_events()
{
    bool shouldStop = false;
    while (XPending(var->display))
    {
        XEvent e;
        XNextEvent(var->display, &e);
        switch (e.type)
        {
            case Expose:
                break;

            case ResizeRequest:
                XWindowAttributes gwa;
                XGetWindowAttributes(var->display, var->window, &gwa);
                if(resizeCallback)
                {
                    resizeCallback(gwa.width, gwa.height);
                }
                break;

            case ClientMessage:
                if (e.xclient.data.l[0] == var->deleteWindowAtom)
                {
                    shouldStop = true;
                }
                break;

            default:
                break;
        }
    }
    return shouldStop || terminated;
}


void XWindow::set_resize_callback(std::function<void(unsigned int, unsigned int)> callbk)
{
    resizeCallback = callbk;

    XWindowAttributes gwa;
    XGetWindowAttributes(var->display, var->window, &gwa);
    if(resizeCallback)
    {
        resizeCallback(gwa.width, gwa.height);
    }
}


void XWindow::create_window()
{
    get_new_visual();

    Window rootWindow = get_root_window(var->display);

    XSetWindowAttributes swa;
    swa.colormap = var->cmap;
    swa.background_pixmap = None ;
    swa.border_pixel      = 0;
    swa.event_mask        = StructureNotifyMask;
    var->window = XCreateWindow(var->display, rootWindow, 0, 0, 600, 600, 0,
                                var->vi->depth, InputOutput, var->vi->visual,
                                CWColormap | CWEventMask, &swa);

    XMapWindow(var->display, var->window);
}


void XWindow::create_gl_context()
{
    if(isExtensionSupported(
            glXQueryExtensionsString(var->display, DefaultScreen(var->display)),
            "GLX_ARB_create_context")
        )
    {
        typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig,
                                                             GLXContext, Bool, const int*);

        glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
                                (glXCreateContextAttribsARBProc)
                                    glXGetProcAddressARB(
                                        (const GLubyte *) "glXCreateContextAttribsARB"
                                    );

        if(glXCreateContextAttribsARB)
        {
            int context_attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
                GLX_CONTEXT_MINOR_VERSION_ARB, 5,
                None
            };

            int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

            var->glc = glXCreateContextAttribsARB(var->display, var->fbc, 0,
                                                 True, context_attribs);

            XSync(var->display, False);
            XSetErrorHandler(oldHandler);

            if(var->glc == nullptr)
            {
                std::cerr << "glXCreateContextAttribsARB failed" << std::endl;
                throw std::runtime_error("glXCreateContextAttribsARB failed");
            }
        }
        else
        {
            std::cerr << "glXCreateContextAttribsARB couldn't be retrieved" << std::endl;
            throw std::runtime_error("glXCreateContextAttribsARB could not be retrieved");
        }
    }
    else
    {
        std::cerr << "glXCreateContextAttribsARB not supported" << std::endl;
        throw std::runtime_error("glXCreateContextAttribsARB not supported");
    }

    glXMakeCurrent(var->display, var->window, var->glc);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        std::cerr << "failed to init glew: "
                  <<  glewGetErrorString(glewError) << std::endl;
        throw std::runtime_error("failed to init glew");
    }
}


void XWindow::get_new_visual()
{
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(var->display, DefaultScreen(var->display),
                                         fb_attribs, &fbcount);
    if(!fbc)
    {
        std::cerr << "failed to retrieve a framebuffer config" << std::endl;
        throw std::runtime_error("failed to retrieve a framebuffer config");
    }

    int best_fbc = -1;
    int worst_fbc = -1;
    int best_num_samp = -1;
    int worst_num_samp = 999;

    for(int i = 0; i < fbcount; ++i)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(var->display, fbc[i]);
        if(vi)
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib(var->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(var->display, fbc[i], GLX_SAMPLES, &samples);

            if (best_fbc < 0 || (samp_buf && samples) > best_num_samp)
            {
                best_fbc = i;
                best_num_samp = samples;
            }
            if(worst_fbc < 0 || (!samp_buf || samples) < worst_num_samp)
            {
                worst_fbc = i;
                worst_num_samp = samples;
            }
        }
        XFree(vi);
    }

    var->fbc = fbc[best_fbc];
    XFree(fbc);

    var->vi = glXGetVisualFromFBConfig(var->display, var->fbc);

    var->cmap = XCreateColormap(var->display,
                              RootWindow( var->display, var->vi->screen ),
                              var->vi->visual, AllocNone );
}

void XWindow::get_visual_from_window()
{
    XWindowAttributes windowAttr;
    XGetWindowAttributes(var->display, var->window, &windowAttr);

    XVisualInfo vi_in;
    int vi_out = 0;
    vi_in.visualid = XVisualIDFromVisual(windowAttr.visual);
    var->vi = XGetVisualInfo(var->display, VisualIDMask,
                                      &vi_in, &vi_out);

    var->cmap = XCreateColormap(var->display, var->window, var->vi->visual, AllocNone);

    int fbcount;
    GLXFBConfig* fbs = glXGetFBConfigs(var->display, DefaultScreen(var->display), &fbcount);
    for(int i = 0; i < fbcount; ++i)
    {
        int visualId;
        glXGetFBConfigAttrib(var->display, fbs[i], GLX_VISUAL_ID, &visualId);
        if((unsigned int) visualId == var->vi->visualid)
        {
            var->fbc = fbs[i];
            break;
        }
    }
    XFree(fbs);
}


void XWindow::set_window_name(const char* s)
{
    XStoreName(var->display, var->window, s);
}


namespace
{
    Window get_root_window(Display* display)
    {
        int screen_num = DefaultScreen(display);
        Screen *screen = XScreenOfDisplay(display, screen_num);
        return RootWindow(display, XScreenNumberOfScreen(screen));
    }


    Window get_virtual_root_window(Display* display)
    {
        return VirtualRootWindowOfScreen(DefaultScreenOfDisplay(display));
    }


    Atom get_delete_window_atom(Display* display, Window window)
    {
        Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &wmDeleteMessage, 1);
        return wmDeleteMessage;
    }


    int isExtensionSupported(const char *extList, const char *extension)
    {
        const char *start;
        const char *where, *terminator;

        where = strchr(extension, ' ');
        if ( where || *extension == '\0' )
            return 0;

        for(start = extList; ;)
        {
            where = strstr( start, extension );

            if ( !where )
                break;

            terminator = where + strlen( extension );

            if ( where == start || *(where - 1) == ' ' )
                if ( *terminator == ' ' || *terminator == '\0' )
                    return 1;

           start = terminator;
       }
       return 0;
    }


    int ctxErrorHandler(Display *dpy, XErrorEvent *ev)
    {
        std::cerr << "Failed to create gl context" << std::endl;
        return 0;
    }
}
