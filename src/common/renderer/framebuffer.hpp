#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP


#include "texture.hpp"

#include <GL/glew.h>
#include <GL/gl.h>

#include <string>


class Framebuffer

{
public:

    Framebuffer(const Texture& texture);
    ~Framebuffer();

    void bind() const;
    static void unbind();

    unsigned int get_width() const;
    unsigned int get_height() const;

private:

    GLuint framebuffer;
    GLuint depthbuffer;
    GLenum drawbuffer;

    unsigned int width;
    unsigned int height;
};


#endif
