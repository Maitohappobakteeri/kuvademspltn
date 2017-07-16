#ifndef RENDERER_TEXTURE_HPP
#define RENDERER_TEXTURE_HPP

#include <GL/glew.h>
#include <GL/gl.h>

#include <string>

class Texture
{
public:

    Texture(Texture* placeholder=nullptr);
    Texture(GLuint ID);
    Texture(const std::string& filePath, bool loadNow=true, Texture* placeholder=nullptr);
    Texture(Texture&& texture);

    Texture(const Texture&) = delete;

    ~Texture();

    static Texture create_empty(unsigned int width, unsigned height);

    Texture& operator=(const Texture&) = delete;

    void set_texture(GLuint ID);

    void bind() const;
    bool load(Texture* placeholder=nullptr);

    std::string get_path() const;
    unsigned int get_width() const;
    unsigned int get_height() const;
    GLuint get_texture() const;

    // Return true if Texture has been read from a file with success
    bool is_loaded() const;

private:

    GLuint load_texture(const std::string& filePath);

    std::string filePath;

    GLuint texture;

    unsigned int width;
    unsigned int height;

    bool isLoaded;
};

#endif
