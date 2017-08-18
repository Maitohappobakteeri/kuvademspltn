#include "buffer.hpp"


Buffer::Buffer()
{
    glGenBuffers(1, &buffer);
}


Buffer::~Buffer()
{
    glDeleteBuffers(1, &buffer);
}


void Buffer::bind()
{
    glBindBuffer(target, buffer);
}


void Buffer::bind(GLenum target)
{
    glBindBuffer(target, buffer);
}


void Buffer::buffer_data(GLenum target, const GLvoid* data,
                         GLsizeiptr size, GLenum usage)
{
    this->target = target;
    bind();
    glBufferData(target, size, data, usage);
    unbind();
}


void Buffer::buffer_sub_data(const GLvoid* data, GLintptr offset, GLsizeiptr size)
{
    bind();
    glBufferSubData(this->target, offset, size, data);
    unbind();
}


void Buffer::unbind()
{
    glBindBuffer(target, 0);
}


void Buffer::unbind(GLenum target)
{
    glBindBuffer(target, 0);
}
