
#include "project.hpp"
#include "print.hpp"
#include "demo.hpp"
#include "resid.hpp"
#include "window/window.hpp"
#include "renderer/renderer.hpp"
#include "renderer/font.hpp"

#include <chrono>
#include <thread>
#include <memory>

class TextDemo : public Demo
{
public:


    TextDemo(const Demo::Args& args, const std::wstring& command)
        :Demo(args, command)
    {

    }


    virtual ~TextDemo()
    {

    }


protected:

    virtual bool init() override
    {
        if(!Demo::init()) return false;

            // load font
    font = renderer->load_font(RES_COMMON_FONT_INCONSOLATA_REGULAR, 64);

    // test string
    std::wstring wstr(L"The garden strawberry (or simply strawberry; Fragaria × ananassa)[1] is a widely grown hybrid species of the genus Fragaria, collectively known as the strawberries, which are cultivated worldwide for their fruit. The fruit is widely appreciated for its characteristic aroma, bright red color, juicy texture, and sweetness. "
                      L"\n\n...");

    // line dimensions
    const float lineWidth = 1.0f;
    const float lineHeight = 0.05f;
    text = std::shared_ptr<Text>( new Text (font.get(), wstr, lineHeight, {0, 0.6f}, {lineWidth, lineHeight}, 0));
    return true;
    }


    virtual void cleanup() override
    {
        font.reset();
        text.reset();
        Demo::cleanup();
    }

    virtual bool update(float step) override
    {
        return false;
    }


    virtual void render() override
    {
        
    const float lineWidth = 1.0f;
    const float lineHeight = 0.05f;

        renderer->clear();

        renderer->render_string_box(font.get(),
                                     L"XxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxX",
                                     {1.0f, 0.1f, 0.0f, 1.0f},
                                     {0,0}, {1, 1}, 0,
                                     Align::BOTTOM_RIGHT);

        renderer->render_string_box(font.get(),
                                     L".........................................................",
                                     {0.7f, 1.0f, 0.0f, 1.0f},
                                     {0,0.75f+lineHeight*2*2},
                                     {lineWidth, lineHeight}, 0);

        renderer->render_string_line(font.get(),
                                    L".........................................................",
                                    {0.7f, 1.0f, 0.0f, 1.0f},
                                    {0,0.75f+lineHeight*2},
                                    {lineWidth, lineHeight}, 0,
                                    Align::RIGHT);
        // render text
        renderer->render_text(*text);

        renderer->render_string_line(font.get(),
                                     L".........................................................",
                                     {0.7f, 1.0f, 0.0f, 1.0f},
                                     {0,-0.75f},
                                     {lineWidth, lineHeight}, 0);
    }


private:
    std::shared_ptr<Font> font;
    std::shared_ptr<Text> text;
};


int main(int argc, char* argv[])
{
    println(PROJECT_NAME, " - text");   
    Demo::Args args = Demo::create_args();
    TextDemo demo(args, join_command(argc, argv));
    exit(demo.run());
}
