#ifndef RENDERER_RENDERER_HPP
#define RENDERER_RENDERER_HPP


#include "color.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "spritegroup.hpp"
#include "font.hpp"
#include "text.hpp"
#include "relative.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/mat4x4.hpp>

#include <string>
#include <memory>
#include <map>


class Renderer
{
public:

    Renderer();
    ~Renderer();

    std::shared_ptr<Texture> load_texture(const std::string& filePath);
    std::shared_ptr<Texture> load_texture(unsigned int ID);

    std::shared_ptr<Font> load_font(const std::string& filePath, unsigned int size=16);
    std::shared_ptr<Font> load_font(unsigned int ID, unsigned int size=16);

    void clear_screen();
    void render_spritegroup(const SpriteGroup& sgroup);

    // Render string scaled to fit inside bow with size
    void render_string_box(Font const* font, const std::wstring str, const Color& color,
                           const glm::vec2& position, const glm::vec2& size, float rotation,
                           Align align=Align::CENTER);

    // Render string scaled to fit inside a line
    void render_string_line(Font const* font, const std::wstring str, const Color& color,
                            const glm::vec2& position, const glm::vec2& size, float rotation,
                            Align align=Align::CENTER);

    void render_text(const Text& text);

    void render_texture(Texture const* texture, const glm::vec2& position,
                        const glm::vec2& size, float rotation);

    void render_rectangle(const Color& color, const glm::vec2& position,
                          const glm::vec2& size, float rotation);
    void render_rectangle_outline(const Color& color, const glm::vec2& position,
                                  const glm::vec2& size, float rotation);
    void render_circle(const Color& color, const glm::vec2& position, const glm::vec2& size);
    void render_line(const Color& color, const glm::vec2& start, const glm::vec2& end);

    void set_render_target_screen();
    void set_render_target(const Framebuffer& fb);

    void handle_resize(unsigned int newWidth, unsigned int newHeight);
    glm::vec2 get_view_scale() const;

    glm::mat4 get_projection_matrix() const;

private:

    void delete_texture(Texture* texture);
    void delete_font(Font* texture);

    void init_gl();
    void render_sprite(const Sprite& sprite);
    void resize();

    void render_char(const Font::Char& ch, const Color& color,
                     const glm::mat4& stringMat=glm::mat4());

    GLuint vao;

    // vertex buffer for a rectangle from (-1,-1,0) to (1,1,0)
    std::unique_ptr<Buffer> rectTriangleStrip;
    std::unique_ptr<Buffer> rectLineStrip;
    // vertex buffer for a circle
    std::unique_ptr<Buffer> circleLineStrip;
    // a line
    std::unique_ptr<Buffer> line;
    // texture mapping that uses the full texture
    std::unique_ptr<Buffer> fullUvBuffer;

    std::unique_ptr<Shader> colorShader;
    std::unique_ptr<Shader> textureShader;

    std::map<unsigned int, std::weak_ptr<Texture>> textures;
    std::unique_ptr<Texture> placeholderTexture;

    std::map<unsigned int, std::map<unsigned int, std::weak_ptr<Font>>> fonts;
    std::shared_ptr<Font> placeholderFont;

    glm::mat4 projectionMatrix;

    glm::vec2 viewScale;
    unsigned int width;
    unsigned int height;

    bool usingFramebuffer;
};

#endif
