
#include "project.hpp"
#include "print.hpp"

#include "window.hpp"
#include "renderer/renderer.hpp"

#include <chrono>
#include <thread>


void render_fire(Shader& shader, Buffer& buf, const Color& col, const glm::mat4& projectionMatrix)
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glEnableVertexAttribArray(0);

    shader.use();

    buf.bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    static int mvpId = shader.get_uniform_location("mvpMatrix");
    static int color = shader.get_uniform_location("inColor");
    static int timeId = shader.get_uniform_location("time");
    glUniform4f(color, col.r, col.g, col.b, col.a);
    glUniform1f(timeId, SDL_GetTicks() / 1000.0f);

    glm::mat4 modelMat = glm::mat4();
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


int main()
{
    println(PROJECT_NAME, " - fire");

    // initialize rendering
    SDL_Init(SDL_INIT_VIDEO);
    Window* window = new Window();
    Renderer* renderer = new Renderer();
    window->set_resize_callback([renderer](unsigned int w, unsigned int h)
                                {renderer->handle_resize(w,h);});

    Shader* fireShader = new Shader("res/fire/shader/fire.fragmentshader", GL_FRAGMENT_SHADER,
                                    "res/fire/shader/fire.vertexshader", GL_VERTEX_SHADER);

    const GLfloat rectpoints[] =
    {
        -1.0,-1.0,0.0,
        1.0,-1.0,0.0,
        -1.0,1.0,0.0,
        1.0f,1.0f,0.0
    };

    Buffer* rectTriangleStrip = new Buffer();
    rectTriangleStrip->buffer_data(GL_ARRAY_BUFFER, rectpoints,
                                   sizeof(rectpoints), GL_STATIC_DRAW);

    // render fire
    while(!window->handle_events())
    {
        renderer->clear_screen();
        render_fire(*fireShader, *rectTriangleStrip, {1,0,0,1}, renderer->get_projection_matrix());
        window->display();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // cleanup
    delete fireShader;
    delete rectTriangleStrip;
    delete renderer;
    delete window;
    SDL_Quit();
}
