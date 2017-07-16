#ifndef RENDERER_BUFFER_HPP
#define RENDERER_BUFFER_HPP

#include <GL/glew.h>
#include <GL/gl.h>

class Buffer
{
public:

    Buffer();
    ~Buffer();

    void bind();
    void bind(GLenum target);
    void buffer_data(GLenum target, const GLvoid* data, GLsizeiptr size, GLenum usage);
    void buffer_sub_data(const GLvoid* data, GLintptr offset, GLsizeiptr size);

    void unbind();
    static void unbind(GLenum target);

private:

    GLuint buffer;
    GLuint target;
};

#endif
