#ifndef RENDERER_FONT_HPP
#define RENDERER_FONT_HPP


#include "texture.hpp"
#include "buffer.hpp"

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <glm/vec2.hpp>


class Font
{
public:

    struct Char
    {
        wchar_t ch;
        Texture const* texture;
        Buffer* uvBuffer;
        unsigned int uvOffset;

        glm::vec2 position;
        glm::vec2 scale;

        float advance;
    };


    Font(const std::string& filePath, unsigned int size);
    ~Font();

    // Find index of last character that doesn't fit into lineWidth
    //     Set first character not on current line to last
    //     Set first character on next line to next
    void break_string(const std::wstring& wstr, float lineWidth,
                      unsigned int& last, unsigned int& next, unsigned int start=0) const;

    static void initialize_freetype();
    static void release_freetype();

    std::string get_path() const;
    unsigned int get_size() const;

    std::vector<Texture const*> get_textures() const;
    Font::Char get_character(wchar_t ch) const;

    // Return true if Font has been read from a file with success
    bool is_loaded() const;

private:

    void load_font(const std::string& filename, unsigned int size);

    std::string filePath;
    unsigned int size;
    bool isLoaded;

    std::map<wchar_t, Char> characters;

    std::vector<std::unique_ptr<Texture>> textures;
    std::unique_ptr<Buffer> uvBuffer;
};


#endif
