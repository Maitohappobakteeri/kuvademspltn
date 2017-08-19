#include "disintdemo.hpp"


#include "print.hpp"
#include "resid.hpp"
#include "rng.hpp"

#include <glm/gtc/matrix_transform.hpp>


DisintDemo::DisintDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command)
{
    // renderDemoInfo = false;
}


DisintDemo::~DisintDemo()
{

}


bool DisintDemo::init()
{
    if(!Demo::init()) return false;

    disintShader = new Shader("res/disintegrate/shader/disint.fragmentshader", GL_FRAGMENT_SHADER,
                              "res/disintegrate/shader/disint.vertexshader", GL_VERTEX_SHADER);

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

    const GLfloat uvpoints[] =
    {
        0.0,0.0,
        1.0,0.0,
        0.0,1.0,
        1.0f,1.0f,
    };

    fullUvBuffer = new Buffer();
    fullUvBuffer->buffer_data(GL_ARRAY_BUFFER, uvpoints,
                              sizeof(uvpoints), GL_STATIC_DRAW);

    texture = renderer->load_texture(RES_COMMON_PLACEHOLDER);

    return true;
}


void DisintDemo::cleanup()
{
    delete disintShader;
    delete rectTriangleStrip;
    delete fullUvBuffer;

    texture.reset();

    Demo::cleanup();
}


bool DisintDemo::update(float step)
{
    return false;
}


void DisintDemo::render()
{
    renderer->clear();
    render_disint();
}


void DisintDemo::render_disint() const
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    disintShader->use();
    texture->bind();

    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    fullUvBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);

    static int mvpId = disintShader->get_uniform_location("mvpMatrix");
    static int colorId = disintShader->get_uniform_location("disintColor");
    static int timeId = disintShader->get_uniform_location("time");
    glUniform4f(colorId, 0.2,0.1,0.6,1);
    glUniform1f(timeId, (SDL_GetTicks() % (1000 * 10)) / 1000.0f);

    glm::mat4 modelMat = glm::mat4();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((renderer->get_projection_matrix()
                                              * modelMat)[0][0]));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
