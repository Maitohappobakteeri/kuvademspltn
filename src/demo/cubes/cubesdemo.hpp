#ifndef DEMO_CUBES_CUBESDEMO_HPP
#define DEMO_CUBES_CUBESDEMO_HPP


#include "demo.hpp"

#include <string>


class CubesDemo : public Demo
{
public:

    CubesDemo(const Demo::Args& args, const std::wstring& command);
    ~CubesDemo();

protected:

    virtual bool init() override;
    virtual void cleanup() override;

    virtual bool update(float step) override;
    virtual void render() override;

    virtual void handle_resize(unsigned int w, unsigned int h) override;

private:

    void render_cube();

    Shader* colorShader;
    Buffer* cubeBuffer;
    Buffer* normalBuffer;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    float zRotation;
};

#endif
