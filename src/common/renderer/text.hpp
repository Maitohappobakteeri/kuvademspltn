#ifndef RENDERER_TEXT_HPP
#define RENDERER_TEXT_HPP


#include "font.hpp"

#include <glm/glm.hpp>

#include <string>


// Text block that expands down (currently)
class Text
{
public:

    struct Line
    {
        std::wstring::const_iterator start;
        std::wstring::const_iterator end;

        glm::vec2 position;
        glm::vec2 size;
    };

    Text(Font* font, const std::wstring& content, float lineHeight,
         const glm::vec2& position, const glm::vec2& size, float rotation);
    ~Text();

    unsigned int lines_size() const;
    Line get_line(unsigned int line) const;

    Font* get_font() const;
    glm::vec2 get_position() const;
    glm::vec2 get_size() const;
    float get_rotation() const;

private:

    void break_lines();

    Font* font;
    std::wstring content;

    float lineHeight;
    glm::vec2 position;
    glm::vec2 size;
    float rotation;

    std::vector<Line> lines;
};


#endif
