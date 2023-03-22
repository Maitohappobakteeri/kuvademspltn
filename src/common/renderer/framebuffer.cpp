#include "framebuffer.hpp"


#include <stdexcept>


Framebuffer::Framebuffer(const Texture& texture)
    :width(texture.get_width()), height(texture.get_height())
{
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.get_texture(), 0);

    drawbuffer = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, &drawbuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("Failed to create framebuffer");
    }
}


Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteRenderbuffers(1, &depthbuffer);
}


void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}


void Framebuffer::unbind() //static
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


unsigned int Framebuffer::get_width() const
{
    return width;
}


unsigned int Framebuffer::get_height() const
{
    return height;
}
