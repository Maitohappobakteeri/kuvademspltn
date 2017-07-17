
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

    glm::mat4 modelMat = glm::mat4();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


class FireDemo : public Demo
{
public:


    FireDemo(bool useRoot)
        :Demo(useRoot)
    {
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

        rainbow1.advance(1.0f);
    }


    virtual ~FireDemo()
    {
        delete fireShader;
        delete rectTriangleStrip;
    }


protected:


    virtual bool update(float step) override
    {
        rainbow1.advance(step);
        rainbow2.advance(step);
        return false;
    }


    virtual void render() override
    {
        renderer->clear_screen();
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
    println(PROJECT_NAME, " - fire");

    bool isScreensaver = false;
    if(argc == 2 && std::string(argv[1]) == "-root")
    {
        isScreensaver = true;
    }

    FireDemo demo(isScreensaver);
    exit(demo.run());
}
