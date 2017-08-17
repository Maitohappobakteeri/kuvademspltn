#include "renderer.hpp"


#include "resid.hpp"
#include "print.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL.h>

#include <algorithm>
#include <iostream>
#include <cmath>


Renderer::Renderer()
{
    init_gl();
    Font::initialize_freetype();

    placeholderTexture.reset(new Texture(get_res_from_ID(RES_COMMON_PLACEHOLDER)));

    const std::string fontFilename = get_res_from_ID(RES_COMMON_FONT_INCONSOLATA_REGULAR);
    placeholderFont = std::shared_ptr<Font>(new Font(fontFilename, 32));

    width = height = 640;
    resize();
}


Renderer::~Renderer()
{
    placeholderTexture.reset();
    placeholderFont.reset();

    while(spritegroups.size() != 0)
    {
        delete_spritegroup(*spritegroups.begin());
    }

    while(textures.size() != 0)
    {
        delete_texture(textures.begin()->second.lock().get());
    }

    while(fonts.size() != 0)
    {
        delete_font(fonts.begin()->second.begin()->second.lock().get());
    }

    Font::release_freetype();

    colorShader.reset();
    textureShader.reset();
    rectTriangleStrip.reset();
    rectLineStrip.reset();
    circleLineStrip.reset();
    line.reset();
    fullUvBuffer.reset();
    glDeleteVertexArrays(1, &vao);
}


std::shared_ptr<Texture> Renderer::load_texture(const std::string& filePath)
{
    return load_texture(get_res_ID(filePath));
}


std::shared_ptr<Texture> Renderer::load_texture(unsigned int ID)
{
    if(textures.find(ID) != textures.end())
    {
        return textures[ID].lock();
    }
    else
    {
        Texture* newTexture = new Texture(get_res_from_ID(ID), true, placeholderTexture.get());
        auto deleteFun = [this](Texture* tex)
        {
            delete_texture(tex);
        };

        std::shared_ptr<Texture> sharedPtr(newTexture, deleteFun);
        textures[ID] = sharedPtr;

        return sharedPtr;
    }
}


void Renderer::delete_texture(Texture* texture)
{
    auto it = textures.find(get_res_ID(texture->get_path()));
    if(it != textures.end())
    {
        delete texture;
        textures.erase(it);
    }
}


std::shared_ptr<Font> Renderer::load_font(const std::string& filePath, unsigned int size)
{
    return load_font(get_res_ID(filePath), size);
}


std::shared_ptr<Font> Renderer::load_font(unsigned int ID, unsigned int size)
{
    if(fonts.find(ID) != fonts.end() && fonts[ID].find(size) != fonts[ID].end())
    {
        return fonts[ID][size].lock();
    }
    else
    {
        Font* newFont = new Font(get_res_from_ID(ID), size);
        auto deleteFun = [this](Font* fnt)
        {
            delete_font(fnt);
        };

        std::shared_ptr<Font> sharedPtr(newFont, deleteFun);

        if(sharedPtr->is_loaded())
        {
            fonts[ID][size] = sharedPtr;
            return sharedPtr;
        }
        else
        {
            return placeholderFont;
        }
    }
}


void Renderer::delete_font(Font* font)
{
    auto itPath = fonts.find(get_res_ID(font->get_path()));
    if(itPath != fonts.end())
    {
        auto itSize = itPath->second.find(font->get_size());
        if(itSize != itPath->second.end())
        {
            delete font;
            itPath->second.erase(itSize);
        }
        if(itPath->second.empty())
        {
            fonts.erase(itPath);
        }
    }
}


SpriteGroup* Renderer::create_spritegroup()
{
    SpriteGroup* newGroup = new SpriteGroup(this);
    spritegroups.push_back(newGroup);
    return newGroup;
}


void Renderer::delete_spritegroup(SpriteGroup* sgroup)
{
    auto it = std::find(spritegroups.begin(), spritegroups.end(), sgroup);
    if(it != spritegroups.end())
    {
        spritegroups.erase(it);
        delete sgroup;
    }
}


void Renderer::clear_screen()
{
    // glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    glClearColor(0, 0, 0, 1.0f);
    glClearDepth(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
}


void Renderer::render_sprites()
{
    Sprite::set_reference_time(SDL_GetTicks());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    //glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);

    textureShader->use();

    static int mvpId = textureShader->get_uniform_location("mvpMatrix");
    static int shadeID = textureShader->get_uniform_location("shadeColor");
    static int alphaID = textureShader->get_uniform_location("alpha");
    glUniform4f(shadeID, 0,0,0,0);
    glUniform1f(alphaID, 1);

    fullUvBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)(0*2*4*sizeof(GLfloat)*33));

    for(SpriteGroup* sgroup : spritegroups)
    {
        for(Sprite* sprite : sgroup->sprites)
        {
            if(!sgroup->is_sprite_visible(*sprite, sgroup->extended_size(viewScale))) continue;

            sprite->get_texture()->bind();

            const glm::vec2 scale = sprite->get_scale() / sgroup->get_scale();
            const glm::vec2 pos = (sprite->get_position() - sgroup->get_position())
                                  / sgroup->get_scale();
            glm::mat4 modelMat = glm::translate(glm::mat4(), {pos.x,pos.y, 0})
                                * glm::rotate(glm::mat4(), sprite->get_rotation(),
                                              {0.0f, 0.0f, -1.0f})
                                * glm::scale(glm::mat4(), {scale.x, scale.y, 1});
            glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
}


void Renderer::render_char(const Font::Char& ch, const Color& color, const glm::mat4& stringMat)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    textureShader->use();

    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    ch.uvBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)(ch.uvOffset*sizeof(GLfloat)));

    static int mvpId = textureShader->get_uniform_location("mvpMatrix");
    static int shadeID = textureShader->get_uniform_location("shadeColor");
    static int alphaID = textureShader->get_uniform_location("alpha");
    glUniform4f(shadeID, color.r, color.g, color. b, 1.0f);
    glUniform1f(alphaID, color.a);

    ch.texture->bind();

    glm::mat4 modelMat = stringMat
                         * glm::translate(glm::mat4(),
                                          2.0f * glm::vec3(ch.position.x, ch.position.y, 0))
                         * glm::scale(glm::mat4(), {ch.scale.x, ch.scale.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void Renderer::render_string_box(Font const* font, const std::wstring wstr, const Color& color,
                                 const glm::vec2& position, const glm::vec2& size, float rotation,
                                 StringAlign align)
{
    float totalWidth = 0;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        totalWidth += font->get_character(wstr[i]).advance;
    }

    glm::vec2 scaledSize;
    if(size.x / size.y > totalWidth)
    {
        scaledSize.x = totalWidth * size.y;
        scaledSize.y = size.y;
    }
    else
    {
        scaledSize.x = size.x;
        scaledSize.y = size.x / totalWidth;
    }

    glm::vec2 alignedPosition(position);
    // horizontal
    if(align == LEFT || align == TOP_LEFT || align == BOTTOM_LEFT)
    {
        alignedPosition.x -= size.x - scaledSize.x;
    }
    else if(align == RIGHT || align == TOP_RIGHT || align == BOTTOM_RIGHT)
    {
        alignedPosition.x += size.x - scaledSize.x;
    }
    // vertical
    if(align == TOP || align == TOP_LEFT || align == TOP_RIGHT)
    {
        alignedPosition.y += size.y - scaledSize.y;
    }
    else if(align == BOTTOM || align == BOTTOM_LEFT || align == BOTTOM_RIGHT)
    {
        alignedPosition.y -= size.y - scaledSize.y;
    }

    float currentWidth = - totalWidth / 2.0f;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        const Font::Char ch = font->get_character(wstr[i]);
        render_char(ch, color,
                    glm::translate(glm::mat4(),
                                   {alignedPosition.x, alignedPosition.y, 0})
                    * glm::rotate(glm::mat4(), rotation, {0.0f, 0.0f, -1.0f})
                    * glm::scale(glm::mat4(),
                                 {scaledSize.x, scaledSize.y, 1})
                    * (glm::translate(glm::mat4(),
                                      {(currentWidth * 2.0f + ch.advance) / totalWidth, 0, 0})
                       * glm::scale(glm::mat4(),
                                    {1.0f / totalWidth, 1.0f, 1})));
        currentWidth += ch.advance;
    }
}


void Renderer::render_string_line(Font const* font, const std::wstring wstr, const Color& color,
                                  const glm::vec2& position, const glm::vec2& size,
                                  float rotation, StringAlign align)
{
    float totalWidth = 0;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        totalWidth += font->get_character(wstr[i]).advance;
    }

    glm::vec2 scaledSize;
    scaledSize.x = totalWidth * size.y;
    scaledSize.y = size.y;

    glm::vec2 alignedPosition(position);
    if(align == LEFT || align == TOP_LEFT || align == BOTTOM_LEFT)
    {
        alignedPosition.x -= size.x - scaledSize.x;
    }
    else if(align == RIGHT || align == TOP_RIGHT || align == BOTTOM_RIGHT)
    {
        alignedPosition.x += size.x - scaledSize.x;
    }

    float currentWidth = - totalWidth / 2.0f;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        const Font::Char ch = font->get_character(wstr[i]);
        render_char(ch, color,
                    glm::translate(glm::mat4(),
                                   {alignedPosition.x, alignedPosition.y, 0})
                    * glm::rotate(glm::mat4(), rotation, {0.0f, 0.0f, -1.0f})
                    * glm::scale(glm::mat4(),
                                 {scaledSize.x, scaledSize.y, 1})
                    * (glm::translate(glm::mat4(),
                                      {(currentWidth * 2.0f + ch.advance) / totalWidth, 0, 0})
                       * glm::scale(glm::mat4(),
                                    {1.0f / totalWidth, 1.0f, 1})));
        currentWidth += ch.advance;
    }
}


void Renderer::render_text(const Text& text)
{
    for(unsigned int i = 0; i < text.lines_size(); ++i)
    {
        const Text::Line line = text.get_line(i);

        // debug start
        render_rectangle_outline({1,1,1, 0.3f}, line.position, line.size, 0);
        // debug end

        render_string_line(text.get_font(),
                           std::wstring(line.start, line.end),
                           {0.7f, 1.0f, 0.0f, 1.0f},
                           line.position, line.size, text.get_rotation(),
                           Renderer::StringAlign::LEFT);
    }
}

void Renderer::render_texture(Texture const* texture, const glm::vec2& position,
                              const glm::vec2& size, float rotation)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    textureShader->use();

    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    fullUvBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)(0*2*4*sizeof(GLfloat)*33));

    static int mvpId = textureShader->get_uniform_location("mvpMatrix");
    static int shadeID = textureShader->get_uniform_location("shadeColor");
    static int alphaID = textureShader->get_uniform_location("alpha");
    glUniform4f(shadeID, 0,0,0,0);
    glUniform1f(alphaID, 1);


    texture->bind();

    glm::mat4 modelMat = glm::translate(glm::mat4(), {position.x,position.y, 0})
                         * glm::rotate(glm::mat4(), rotation,
                                      {0.0f, 0.0f, -1.0f})
                         * glm::scale(glm::mat4(), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void Renderer::render_rectangle(const Color& color, const glm::vec2& position,
                                const glm::vec2& size, float rotation)
{
    glEnableVertexAttribArray(0);

    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    colorShader->use();

    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");

    glUniform4f(colId, color.r, color.g, color.b, color.a);

    glm::mat4 modelMat = glm::translate(glm::mat4(), {position.x, position.y, 0})
                         * glm::rotate(glm::mat4(),rotation, {0.0f, 0.0f, -1.0f})
                         * glm::scale(glm::mat4(), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void Renderer::render_rectangle_outline(const Color& color, const glm::vec2& position,
                                        const glm::vec2& size, float rotation)
{
    glEnableVertexAttribArray(0);

    rectLineStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    colorShader->use();

    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");

    glUniform4f(colId, color.r, color.g, color.b, color.a);

    glm::mat4 modelMat = glm::translate(glm::mat4(), {position.x, position.y, 0})
                         * glm::rotate(glm::mat4(),rotation, {0.0f, 0.0f, -1.0f})
                         * glm::scale(glm::mat4(), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_LINE_LOOP, 0, 4);
}


void Renderer::render_circle(const Color& color, const glm::vec2& position, const glm::vec2& size)
{
    glEnableVertexAttribArray(0);

    circleLineStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    colorShader->use();

    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");

    static glm::mat4 plainMatrix;

    glUniform4f(colId, color.r, color.g, color.b, color.a);

    glm::mat4 modelMat = glm::translate(glm::mat4(), {position.x, position.y, 0})
                        * glm::scale(glm::mat4(), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_LINE_STRIP, 0, 12);
}


void Renderer::render_line(const Color& color, const glm::vec2& start, const glm::vec2& end)
{
    glEnableVertexAttribArray(0);

    line->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    colorShader->use();

    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");

    static glm::mat4 plainMatrix;

    glUniform4f(colId, color.r, color.g, color.b, color.a);

    glm::vec2 position = (end + start) / 2.0f;

    glm::vec2 diff = end - start;
    float scale = glm::length(diff) / 2.0f;

    const float PI = 3.14159265359f;
    float rotation = PI / 2.0f;
    if(diff.x != 0)
    {
        rotation = std::atan(diff.y / diff.x);
        printstatus(rotation);
    }

    glm::mat4 modelMat = glm::translate(glm::mat4(), {position.x, position.y, 0})
                         * glm::rotate(glm::mat4(), rotation, {0.0f, 0.0f, 1.0f})
                         * glm::scale(glm::mat4(), {scale, scale, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    glDrawArrays(GL_LINE_STRIP, 0, 2);
}


void Renderer::init_gl()
{
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    println("OpenGL version ", major, ".", minor);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLfloat rectpoints[] =
    {
        -1.0,-1.0,0.0,
        1.0,-1.0,0.0,
        -1.0,1.0,0.0,
        1.0f,1.0f,0.0
    };

    const GLfloat rectlinepoints[] =
    {
        -1.0,-1.0,0.0,
        -1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0f,-1.0f,0.0
    };

    const GLfloat uvpoints[] =
    {
        0.0,0.0,
        1.0,0.0,
        0.0,1.0,
        1.0f,1.0f,
    };

    rectTriangleStrip.reset(new Buffer());
    rectTriangleStrip->buffer_data(GL_ARRAY_BUFFER, rectpoints,
                                   sizeof(rectpoints), GL_STATIC_DRAW);

    rectLineStrip.reset(new Buffer());
    rectLineStrip->buffer_data(GL_ARRAY_BUFFER, rectlinepoints,
                               sizeof(rectlinepoints), GL_STATIC_DRAW);

    std::vector<GLfloat> circlePoints;
    const unsigned int numberOfCirclePoints = 12;
    const float fullCirclePart = 3.14159265359f * 2.0f / float(numberOfCirclePoints);
    for(unsigned int i = 0; i < numberOfCirclePoints; ++i)
    {
       circlePoints.push_back(std::cos(float(i) * fullCirclePart));
       circlePoints.push_back(std::sin(float(i) * fullCirclePart));
       circlePoints.push_back(0.0f);
    }

    circleLineStrip.reset(new Buffer());
    circleLineStrip->buffer_data(GL_ARRAY_BUFFER, &circlePoints[0],
                                 circlePoints.size()*sizeof(GLfloat), GL_STATIC_DRAW);

    const GLfloat linepoints[] =
    {
        -1.0,0.0,0.0,
        1.0,0.0,0.0
    };

    line.reset(new Buffer());
    line->buffer_data(GL_ARRAY_BUFFER, &linepoints[0],
                      sizeof(linepoints)*sizeof(GLfloat), GL_STATIC_DRAW);

    fullUvBuffer.reset(new Buffer());
    fullUvBuffer->buffer_data(GL_ARRAY_BUFFER, uvpoints,
                              sizeof(uvpoints), GL_STATIC_DRAW);

    colorShader.reset(new Shader("res/common/shader/color.fragmentshader", GL_FRAGMENT_SHADER,
                                 "res/common/shader/color.vertexshader", GL_VERTEX_SHADER));

    textureShader.reset(new Shader("res/common/shader/texture.fragmentshader", GL_FRAGMENT_SHADER,
                                   "res/common/shader/texture.vertexshader", GL_VERTEX_SHADER));
}


void Renderer::set_render_target_screen()
{
    usingFramebuffer = false;

    Framebuffer::unbind();

    glViewport(0, 0, width, height);
    const double f = double(width) / double(height);
    projectionMatrix = glm::ortho(-f, f, -1.0, 1.0);
    viewScale.x = f;
    viewScale.y = 1.0f;
}


void Renderer::set_render_target(const Framebuffer& fb)
{
    usingFramebuffer = true;

    fb.bind();
    glViewport(0, 0, fb.get_width(), fb.get_height());
    const double f = fb.get_width() / fb.get_height();
    projectionMatrix = glm::ortho(-f, f, -1.0, 1.0);
    viewScale.x = f;
    viewScale.y = 1.0f;
}


void Renderer::handle_resize(unsigned int newWidth, unsigned int newHeight)
{
    width = newWidth;
    height = newHeight;
    resize();
}


glm::vec2 Renderer::get_view_scale() const
{
    return viewScale;
}


glm::mat4 Renderer::get_projection_matrix() const
{
    return projectionMatrix;
}

void Renderer::resize()
{
    if(!usingFramebuffer)
    {
        glViewport(0, 0, width, height);
        const double f = double(width) / double(height);
        projectionMatrix = glm::ortho(-f, f, -1.0, 1.0);
        viewScale.x = f;
        viewScale.y = 1.0f;
    }
}
