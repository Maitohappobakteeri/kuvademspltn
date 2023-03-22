#include "renderer.hpp"


#include "resid.hpp"
#include "print.hpp"
#include "time.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <iostream>
#include <cmath>


Renderer::Renderer()
{
    init_gl();
    Font::initialize_freetype();

    placeholderTexture.reset(new Texture(get_res_from_ID(RES_COMMON_PLACEHOLDER)));

    std::string fontFilename = get_res_from_ID(RES_COMMON_FONT_INCONSOLATA_REGULAR);
    placeholderFont = std::shared_ptr<Font>(new Font(fontFilename, 32));

    width = height = 640; // 640 is just a random starting size
    resize();
}


Renderer::~Renderer()
{
    placeholderTexture.reset();
    placeholderFont.reset();

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


void Renderer::clear()
{
    clear(Color::WHITE);
}


void Renderer::clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClearDepth(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // |vvv| All this should most likely be somewhere else |vvv|
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
}


void Renderer::render_spritegroup(const SpriteGroup& sgroup)
{
    Sprite::set_reference_time(time_ms());

    textureShader->use();

    // model
    glEnableVertexAttribArray(0);
    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    // texture uv
    glEnableVertexAttribArray(1);
    fullUvBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)(0*2*4*sizeof(GLfloat)*33));

    // shader uniforms
    static int mvpId = textureShader->get_uniform_location("mvpMatrix");
    static int shadeID = textureShader->get_uniform_location("shadeColor");
    static int alphaID = textureShader->get_uniform_location("alpha");
    glUniform4f(shadeID, 0,0,0,0);
    glUniform1f(alphaID, 1);

    // SpriteGroup values
    glm::vec2 sgroupPosition = sgroup.get_position();
    glm::vec2 sgroupScale = sgroup.get_scale();
    glm::vec2 sgroupExtendedSize = sgroup.extended_size(viewScale);

    // render all sprites
    for(Sprite const* sprite : sgroup.sprites)
    {
        if(!sgroup.is_sprite_visible(*sprite, sgroupExtendedSize)) continue;

        sprite->get_texture()->bind();

        // calc model matrix
        const glm::vec2 scale = sprite->get_scale() / sgroupScale;
        const glm::vec2 pos = (sprite->get_position() - sgroupPosition)
                              / sgroupScale;
        glm::mat4 modelMat = glm::translate(glm::mat4(1.0), {pos.x,pos.y, 0})
                            * glm::rotate(glm::mat4(1.0), sprite->get_rotation(),
                                          {0.0f, 0.0f, -1.0f})
                            * glm::scale(glm::mat4(1.0), {scale.x, scale.y, 1});
        glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

        // render
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void Renderer::render_char(const Font::Char& ch, const Color& color, const glm::mat4& stringMat)
{
    textureShader->use();

    // model
    glEnableVertexAttribArray(0);
    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    // texture uv
    glEnableVertexAttribArray(1);
    ch.uvBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)(ch.uvOffset*sizeof(GLfloat)));

    // shader uniforms
    static int mvpId = textureShader->get_uniform_location("mvpMatrix");
    static int shadeID = textureShader->get_uniform_location("shadeColor");
    static int alphaID = textureShader->get_uniform_location("alpha");
    glUniform4f(shadeID, color.r, color.g, color. b, 1.0f);
    glUniform1f(alphaID, color.a);

    ch.texture->bind();

    // calc model matrix
    glm::mat4 modelMat = stringMat
                         * glm::translate(glm::mat4(1.0),
                                          2.0f * glm::vec3(ch.position.x, ch.position.y, 0))
                         * glm::scale(glm::mat4(1.0), {ch.scale.x, ch.scale.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    // render
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void Renderer::render_string_box(Font const* font, const std::wstring wstr, const Color& color,
                                 const glm::vec2& position, const glm::vec2& size, float rotation,
                                 Align align)
{
    // sum all character widths
    float totalWidth = 0;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        totalWidth += font->get_character(wstr[i]).advance;
    }

    // scale size to fit box
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

    // align text
    glm::vec2 alignedPosition(position);
    // horizontal
    if(align == Align::LEFT || align == Align::TOP_LEFT || align == Align::BOTTOM_LEFT)
    {
        alignedPosition.x -= size.x - scaledSize.x;
    }
    else if(align == Align::RIGHT || align == Align::TOP_RIGHT || align == Align::BOTTOM_RIGHT)
    {
        alignedPosition.x += size.x - scaledSize.x;
    }
    // vertical
    if(align == Align::TOP || align == Align::TOP_LEFT || align == Align::TOP_RIGHT)
    {
        alignedPosition.y += size.y - scaledSize.y;
    }
    else if(align == Align::BOTTOM || align == Align::BOTTOM_LEFT || align == Align::BOTTOM_RIGHT)
    {
        alignedPosition.y -= size.y - scaledSize.y;
    }

    // render characters
    float currentWidth = - totalWidth / 2.0f;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        const Font::Char ch = font->get_character(wstr[i]);
        render_char(ch, color,
                    glm::translate(glm::mat4(1.0),
                                   {alignedPosition.x, alignedPosition.y, 0})
                    * glm::rotate(glm::mat4(1.0), rotation, {0.0f, 0.0f, -1.0f})
                    * glm::scale(glm::mat4(1.0),
                                 {scaledSize.x, scaledSize.y, 1})
                    * (glm::translate(glm::mat4(1.0),
                                      {(currentWidth * 2.0f + ch.advance) / totalWidth, 0, 0})
                       * glm::scale(glm::mat4(1.0),
                                    {1.0f / totalWidth, 1.0f, 1})));
        currentWidth += ch.advance;
    }
}


void Renderer::render_string_line(Font const* font, const std::wstring wstr, const Color& color,
                                  const glm::vec2& position, const glm::vec2& size,
                                  float rotation, Align align)
{
    // sum all character widths
    float totalWidth = 0;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        totalWidth += font->get_character(wstr[i]).advance;
    }

    // scale width to fit text into line height
    glm::vec2 scaledSize;
    scaledSize.x = totalWidth * size.y;
    scaledSize.y = size.y;

    // align text (only horizontally because height is always filled)
    glm::vec2 alignedPosition(position);
    if(align == Align::LEFT || align == Align::TOP_LEFT || align == Align::BOTTOM_LEFT)
    {
        alignedPosition.x -= size.x - scaledSize.x;
    }
    else if(align == Align::RIGHT || align == Align::TOP_RIGHT || align == Align::BOTTOM_RIGHT)
    {
        alignedPosition.x += size.x - scaledSize.x;
    }

    // render characters
    float currentWidth = - totalWidth / 2.0f;
    for(unsigned int i = 0; i < wstr.size(); ++i)
    {
        const Font::Char ch = font->get_character(wstr[i]);
        render_char(ch, color,
                    glm::translate(glm::mat4(1.0),
                                   {alignedPosition.x, alignedPosition.y, 0})
                    * glm::rotate(glm::mat4(1.0), rotation, {0.0f, 0.0f, -1.0f})
                    * glm::scale(glm::mat4(1.0),
                                 {scaledSize.x, scaledSize.y, 1})
                    * (glm::translate(glm::mat4(1.0),
                                      {(currentWidth * 2.0f + ch.advance) / totalWidth, 0, 0})
                       * glm::scale(glm::mat4(1.0),
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
                           Align::LEFT);
    }
}

void Renderer::render_texture(Texture const* texture, const glm::vec2& position,
                              const glm::vec2& size, float rotation)
{
    textureShader->use();

    // model
    glEnableVertexAttribArray(0);
    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    // texture uv
    glEnableVertexAttribArray(1);
    fullUvBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)(0*2*4*sizeof(GLfloat)*33));

    // shader uniforms
    static int mvpId = textureShader->get_uniform_location("mvpMatrix");
    static int shadeID = textureShader->get_uniform_location("shadeColor");
    static int alphaID = textureShader->get_uniform_location("alpha");
    glUniform4f(shadeID, 0,0,0,0);
    glUniform1f(alphaID, 1);

    texture->bind();

    // calc model matrix
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), {position.x,position.y, 0})
                         * glm::rotate(glm::mat4(1.0), rotation,
                                      {0.0f, 0.0f, -1.0f})
                         * glm::scale(glm::mat4(1.0), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    // render
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void Renderer::render_rectangle(const Color& color, const glm::vec2& position,
                                const glm::vec2& size, float rotation)
{
    colorShader->use();

    // model
    glEnableVertexAttribArray(0);
    rectTriangleStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    // shader uniforms
    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");
    glUniform4f(colId, color.r, color.g, color.b, color.a);

    // calc model matrix
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), {position.x, position.y, 0})
                         * glm::rotate(glm::mat4(1.0),rotation, {0.0f, 0.0f, -1.0f})
                         * glm::scale(glm::mat4(1.0), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    // render
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
}


void Renderer::render_rectangle_outline(const Color& color, const glm::vec2& position,
                                        const glm::vec2& size, float rotation)
{
    colorShader->use();

    glEnableVertexAttribArray(0);
    rectLineStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    // shader uniforms
    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");
    glUniform4f(colId, color.r, color.g, color.b, color.a);

    // calc model matrix
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), {position.x, position.y, 0})
                         * glm::rotate(glm::mat4(1.0),rotation, {0.0f, 0.0f, -1.0f})
                         * glm::scale(glm::mat4(1.0), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    // render
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glDisableVertexAttribArray(0);
}


void Renderer::render_circle(const Color& color, const glm::vec2& position, const glm::vec2& size)
{
    colorShader->use();

    // model
    glEnableVertexAttribArray(0);
    circleLineStrip->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    // shader uniforms
    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");
    glUniform4f(colId, color.r, color.g, color.b, color.a);

    // calc model matrix
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), {position.x, position.y, 0})
                        * glm::scale(glm::mat4(1.0), {size.x, size.y, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    // render
    glDrawArrays(GL_LINE_STRIP, 0, 12);

    glDisableVertexAttribArray(0);
}


void Renderer::render_line(const Color& color, const glm::vec2& start, const glm::vec2& end)
{
    colorShader->use();

    // model
    glEnableVertexAttribArray(0);
    line->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    // shader uniforms
    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int colId = colorShader->get_uniform_location("inColor");
    glUniform4f(colId, color.r, color.g, color.b, color.a);

    // calc line position, scale and rotation
    glm::vec2 position = (end + start) / 2.0f;
    glm::vec2 diff = end - start;
    float scale = glm::length(diff) / 2.0f;

    const float PI = 3.14159265359f;
    float rotation = PI / 2.0f;
    if(diff.x != 0)
    {
        rotation = std::atan(diff.y / diff.x);
    }

    // calc model matrix
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0), {position.x, position.y, 0})
                         * glm::rotate(glm::mat4(1.0), rotation, {0.0f, 0.0f, 1.0f})
                         * glm::scale(glm::mat4(1.0), {scale, scale, 1});
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * modelMat)[0][0]));

    // render
    glDrawArrays(GL_LINE_STRIP, 0, 2);

    glDisableVertexAttribArray(0);
}


void Renderer::init_gl()
{
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    println("OpenGL version ", major, ".", minor);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //
    // buffers
    const GLfloat rectTriangleStripPoints[] =
    {
        -1.0,-1.0,0.0,
        1.0,-1.0,0.0,
        -1.0,1.0,0.0,
        1.0f,1.0f,0.0
    };

    rectTriangleStrip.reset(new Buffer());
    rectTriangleStrip->buffer_data(GL_ARRAY_BUFFER, rectTriangleStripPoints,
                                   sizeof(rectTriangleStripPoints), GL_STATIC_DRAW);

    const GLfloat rectLinePoints[] =
    {
        -1.0,-1.0,0.0,
        -1.0,1.0,0.0,
        1.0,1.0,0.0,
        1.0f,-1.0f,0.0
    };

    rectLineStrip.reset(new Buffer());
    rectLineStrip->buffer_data(GL_ARRAY_BUFFER, rectLinePoints,
                               sizeof(rectLinePoints), GL_STATIC_DRAW);

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

    const GLfloat linePoints[] =
    {
        -1.0,0.0,0.0,
        1.0,0.0,0.0
    };

    line.reset(new Buffer());
    line->buffer_data(GL_ARRAY_BUFFER, linePoints,
                      sizeof(linePoints)*sizeof(GLfloat), GL_STATIC_DRAW);

    const GLfloat uvPoints[] =
    {
      0.0,0.0,
      1.0,0.0,
      0.0,1.0,
      1.0f,1.0f,
    };

    fullUvBuffer.reset(new Buffer());
    fullUvBuffer->buffer_data(GL_ARRAY_BUFFER, uvPoints,
                              sizeof(uvPoints)*sizeof(GLfloat), GL_STATIC_DRAW);

    //
    // shaders
    colorShader.reset(new Shader("res/common/shader/color.fragmentshader", GL_FRAGMENT_SHADER,
                                 "res/common/shader/color.vertexshader", GL_VERTEX_SHADER));
    textureShader.reset(new Shader("res/common/shader/texture.fragmentshader", GL_FRAGMENT_SHADER,
                                   "res/common/shader/texture.vertexshader", GL_VERTEX_SHADER));

    set_render_target_screen();
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
