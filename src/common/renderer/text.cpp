#include "text.hpp"


Text::Text(Font* font, const std::wstring& content, float lineHeight,
           const glm::vec2& position, const glm::vec2& size, float rotation)
    :font(font), content(content), lineHeight(lineHeight),
     position(position), size(size), rotation(rotation)
{
    break_lines();
}


Text::~Text()
{

}


unsigned int Text::lines_size() const
{
    return lines.size();
}


Text::Line Text::get_line(unsigned int line) const
{
    return lines[line];
}


Font* Text::get_font() const
{
    return font;
}


glm::vec2 Text::get_position() const
{
    return position;
}


glm::vec2 Text::get_size() const
{
    return size;
}


float Text::get_rotation() const
{
    return rotation;
}


void Text::break_lines()
{
    lines.clear();
    unsigned int startPos = 0;
    unsigned int pos, nextLineStart;
    font->break_string(content, size.x / size.y, pos, nextLineStart, 0);
    while(pos != content.size())
    {
        lines.push_back({content.begin() + startPos, content.begin() + pos,
                        position + glm::vec2(0, size.y - lineHeight - lineHeight*2*lines.size()),
                        {size.x, lineHeight}});

        startPos = nextLineStart;
        font->break_string(content, size.x / size.y, pos, nextLineStart, nextLineStart);
    }

    lines.push_back({content.begin() + startPos, content.begin() + pos,
                    position + glm::vec2(0, size.y - lineHeight - lineHeight*2*lines.size()),
                    {size.x, lineHeight}});
}
