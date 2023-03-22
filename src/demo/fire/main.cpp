
#include "project.hpp"
#include "print.hpp"

#include "rainbowcycle.hpp"
#include "demo.hpp"
#include "renderer/renderer.hpp"

#include <chrono>
#include <thread>


void render_fire(Shader& shader, Buffer& buf, const Color& pcol, const Color& scol,
                 const glm::mat4& projectionMatrix)
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glEnableVertexAttribArray(0);

    shader.use();

    buf.bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    static int mvpId = shader.get_uniform_location("mvpMatrix");
    static int primColor = shader.get_uniform_location("primaryColor");
    static int secColor = shader.get_uniform_location("secondaryColor");
    static int timeId = shader.get_uniform_location("time");
    glUniform4f(primColor, pcol.r, pcol.g, pcol.b, pcol.a);
    glUniform4f(secColor, scol.r, scol.g, scol.b, scol.a);
    glUniform1f(timeId, SDL_GetTicks() / 1000.0f);

    glm::mat4 modelMat = glm::mat4(1.0);
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


class FireDemo : public Demo
{
public:


    FireDemo(const Demo::Args& args, const std::wstring& command)
        :Demo(args, command)
    {

    }


    virtual ~FireDemo()
    {

    }


protected:

    virtual bool init() override
    {
        if(!Demo::init()) return false;

        fireShader = new Shader("res/fire/shader/fireMult.fragmentshader", GL_FRAGMENT_SHADER,
                                       "res/fire/shader/fire.vertexshader", GL_VERTEX_SHADER);

        const GLfloat rectpoints[] =
        {
           -1.0,-1.0,0.0,
           1.0,-1.0,0.0,
           -1.0,1.0,0.0,
           1.0f,1.0f,0.0
        };

        rectTriangleStrip = new Buffer();
        rectTriangleStrip->buffer_data(GL_ARRAY_BUFFER, rectpoints,
                                      sizeof(rectpoints), GL_STATIC_DRAW);

        rainbow1.advance(0.3f);
        rainbow1.set_saturation_range(0.1, 1, 0.5);
        rainbow1.set_brightness_range(0.8, 1, 0.25);

        rainbow2.set_saturation_range(0.8, 1, 1.0);
        rainbow2.set_brightness_range(0.6, 1, 0.8);

        renderDemoInfo = false;

        return true;
    }


    virtual void cleanup() override
    {
        delete fireShader;
        delete rectTriangleStrip;

        Demo::cleanup();
    }

    virtual bool update(float step) override
    {
        const float colorSpeed = 1.0f / 4.0f;
        rainbow1.advance(step*colorSpeed);
        rainbow2.advance(step*colorSpeed);
        return false;
    }


    virtual void render() override
    {
        renderer->clear();
        render_fire(*fireShader, *rectTriangleStrip, rainbow1.get_color(), rainbow2.get_color(),
                    renderer->get_projection_matrix());
    }


private:

    Shader* fireShader;
    Buffer* rectTriangleStrip;
    RainbowCycle rainbow1;
    RainbowCycle rainbow2;
};


int main(int argc, char* argv[])
{
    Demo::Args args = Demo::create_args();

    // Handle basic options
    if(args.disablePrint)
    {
        disable_print();
    }
    if(args.printHelp)
    {
    }

    if(args.shouldRun)
    {
        println(PROJECT_NAME, " - fire");
        FireDemo demo(args, join_command(argc, argv));
        exit(demo.run());
    }
    else
    {
        exit(0);
    }
}
