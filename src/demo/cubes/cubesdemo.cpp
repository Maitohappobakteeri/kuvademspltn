#include "cubesdemo.hpp"


#include "print.hpp"

#include <glm/gtc/matrix_transform.hpp>


CubesDemo::CubesDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command)
{
    // renderDemoInfo = false;
}


CubesDemo::~CubesDemo()
{

}


bool CubesDemo::init()
{
    if(!Demo::init()) return false;

    colorShader = new Shader("res/cubes/shader/color.fragmentshader", GL_FRAGMENT_SHADER,
                             "res/cubes/shader/color.vertexshader", GL_VERTEX_SHADER);

    const GLfloat cubepoints[] =
    {
        -1.0,-1.0,-1.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,-1.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,-1.0,
        1.0,-1.0,1.0,

        -1.0,1.0,-1.0,
        -1.0,1.0,1.0,
        1.0,1.0,-1.0,
        -1.0,1.0,1.0,
        1.0,1.0,-1.0,
        1.0,1.0,1.0,

        -1.0,-1.0,-1.0,
        1.0,-1.0,-1.0,
        1.0,1.0,-1.0,
        -1.0,-1.0,-1.0,
        1.0,1.0,-1.0,
        -1.0,1.0,-1.0,

        -1.0,-1.0,1.0,
        1.0,-1.0,1.0,
        1.0,1.0,1.0,
        -1.0,-1.0,1.0,
        1.0,1.0,1.0,
        -1.0,1.0,1.0,

        -1.0,-1.0,-1.0,
        -1.0,-1.0,1.0,
        -1.0,1.0,-1.0,
        -1.0,-1.0,1.0,
        -1.0,1.0,-1.0,
        -1.0,1.0,1.0,

        1.0,-1.0,-1.0,
        1.0,-1.0,1.0,
        1.0,1.0,-1.0,
        1.0,-1.0,1.0,
        1.0,1.0,-1.0,
        1.0,1.0,1.0,
    };

    const GLfloat normalpoints[] =
    {
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,

        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,

        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,

        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,

        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,

        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0
    };

    cubeBuffer = new Buffer();
    cubeBuffer->buffer_data(GL_ARRAY_BUFFER, cubepoints,
                            sizeof(cubepoints), GL_STATIC_DRAW);

    normalBuffer = new Buffer();
    normalBuffer->buffer_data(GL_ARRAY_BUFFER, normalpoints,
                              sizeof(normalpoints), GL_STATIC_DRAW);

    viewMatrix = glm::lookAt(glm::vec3{-0.5f,4.0f,-5.0f}, glm::vec3{0.0f,0.0f,0.0f},
                             glm::vec3{0.0f,1.0f,0.0f});

    zRotation = 0;
    viewRotation = 0;

    return true;
}


void CubesDemo::cleanup()
{
    delete colorShader;
    delete cubeBuffer;
    delete normalBuffer;

    Demo::cleanup();
}


bool CubesDemo::update(float step)
{
    zRotation += step;
    viewRotation += step / 3.0f;

    glm::vec4 rotatedPosition = glm::rotate(glm::mat4(), viewRotation,
                                         {glm::cross(glm::vec3{-0.5f,4.0f,-5.0f},
                                                     glm::vec3{0,1,0})})
                                * glm::vec4{-0.5f,4.0f,-5.0f, 1.0};
    viewMatrix = glm::lookAt(glm::vec3(rotatedPosition),
                             glm::vec3{0.0f,0.0f,0.0f},
                             glm::vec3{0.0f,1.0f,0.0f});

    return false;
}


void CubesDemo::render()
{
    renderer->clear_screen();
    // renderer->render_rectangle({0.01,0.025,0.1}, {0,0}, renderer->get_view_scale(), 0);
    render_cube();
}


void CubesDemo::handle_resize(unsigned int w, unsigned int h)
{
    Demo::handle_resize(w, h);

    const glm::vec2 viewScale = renderer->get_view_scale();
    projectionMatrix = glm::perspective(glm::radians(90.0f), viewScale.x/viewScale.y,
                                        0.1f, 100.0f);
}


void CubesDemo::render_cube()
{
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    colorShader->use();

    cubeBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    normalBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colorId = colorShader->get_uniform_location("inColor");
    glUniform4f(colorId, 1.0, 0, 0, 1);

    glm::mat4 modelMat = glm::rotate(glm::mat4(), zRotation, {0.0f, 1.0f, 0.0f});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * viewMatrix * modelMat)[0][0]));

    glDrawArrays(GL_TRIANGLES, 0, 3*2*6);
}
