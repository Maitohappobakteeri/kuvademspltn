#include "font.hpp"


#include "print.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <stdexcept>


namespace
{
    FT_Library library;

    GLuint create_texture(unsigned int size, const std::vector<GLubyte>& bitmapData)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture( GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     &bitmapData[0]);
        return texture;
    }
}


Font::Font(const std::string& filePath, unsigned int size)
    :filePath(filePath), size(0), isLoaded(false)
{
    load_font(filePath, size);
}


Font::~Font()
{

}


void Font::initialize_freetype()
{
    if(FT_Init_FreeType(&library) != FT_Err_Ok)
    {
        throw std::runtime_error("failed to initialize freetype");
    }
}


void Font::release_freetype()
{
    FT_Done_FreeType(library);
}


void Font::break_string(const std::wstring& wstr, float lineWidth, unsigned int& last,
                        unsigned int& next, unsigned int start) const
{
    unsigned int pos = start;
    unsigned int lastPos = start;
    float usedWidth = 0;

    for(unsigned int i = start; i < wstr.size(); ++i)
    {
        usedWidth += get_character(wstr[i]).advance;

        if(wstr[i] == '\n')
        {
            // stop line at newline
            last = i;

            // find first character that isn't space
            unsigned int n = last + 1;
            while(wstr.size() > n && wstr[n] == ' ')
            {
                ++n;
            }
            next = n;

            return;
        }
        else if(wstr[i] == ' ' || wstr[i] == '-')
        {
            // word has ended, save lastPos for last
            if(pos != i)
            {
                lastPos = i;
            }

            // increment pos for next
            pos = i + 1;
        }
        else
        {
            if(usedWidth >= lineWidth)
            {
                if(lastPos != start)
                {
                    last = lastPos;
                    next = pos;
                }
                else
                {
                    // if no good line end was found, set last and next to current index
                    last = next = i;
                }
                return;
            }
        }
    }

    // string ended
    last = next =  wstr.size();
}


std::string Font::get_path() const
{
    return filePath;
}


unsigned int Font::get_size() const
{
    return size;
}


std::vector<Texture const*> Font::get_textures() const
{
    std::vector<Texture const*> pointers;
    for(const std::unique_ptr<Texture>& uniqPtr : textures)
    {
        pointers.push_back(uniqPtr.get());
    }
    return pointers;
}


Font::Char Font::get_character(wchar_t ch) const
{
    auto it = characters.find(ch);
    if(it == characters.end())
    {
        return characters.at('?');
    }
    else
    {
        return it->second;
    }
}


bool Font::is_loaded() const
{
    return isLoaded;
}


void Font::load_font(const std::string& filename, unsigned int size)
{
    println("loading font: ", filename);

    const int TEX_SIZE = 1024;
    FT_Error error;

    FT_Face face;
    error = FT_New_Face(library, filename.c_str(), 0, &face);
    if(error == FT_Err_Unknown_File_Format)
    {
        // add some error reaction here
        return;
    }
    else if(error != FT_Err_Ok)
    {
        // add some error reaction here
        return;
    }

    error = FT_Set_Char_Size(face, 0, size*64, 300, 300);
    if(error != FT_Err_Ok)
    {
        // add some error reaction here
        return;
    }

    FT_ULong  charcode;
    FT_UInt   gindex;

    // create vector for data, initialized to 0
    std::vector<GLubyte> bitmapData(4 * TEX_SIZE * TEX_SIZE, 0);
    std::vector<GLfloat> uvData;
    Texture* texture = new Texture();
    uvBuffer.reset(new Buffer());

    int posX = 0;
    int posY = 0;

    int lineHeight = face->size->metrics.height >> 6;

    int maxHeight = 0;
    charcode = FT_Get_First_Char(face, &gindex);
    while (gindex != 0)
    {
        if(FT_Load_Glyph(face, gindex, FT_LOAD_DEFAULT) == FT_Err_Ok
           && FT_Render_Glyph(face->glyph,  FT_RENDER_MODE_NORMAL) == FT_Err_Ok)
        {
            int height = face->glyph->metrics.height >> 6;
            int width = face->glyph->metrics.width >> 6;
            int advance = face->glyph->advance.x >> 6;

            if(posX + width >= TEX_SIZE)
            {
                posX = 0;
                posY += maxHeight;
            }
            if(posY + height >= TEX_SIZE)
            {
                texture->set_texture(create_texture(TEX_SIZE, bitmapData));
                textures.emplace_back(texture);
                texture = new Texture();

                for(unsigned int i = 0; i < 4 * TEX_SIZE * TEX_SIZE; ++i)
                {
                    bitmapData[i] = 0;
                }

                posX = 0;
                posY = 0;
                maxHeight = 0;
            }
            maxHeight = std::max(maxHeight, height);

            // copy pixels to bitmapData
            for(unsigned int y = 0; y < face->glyph->bitmap.rows; ++y)
            {
                for(unsigned int x = 0; x < face->glyph->bitmap.width; ++x)
                {
                    unsigned int bitmapPos = 4*((posY + y) * TEX_SIZE + posX + x);
                    GLubyte byte = face->glyph->bitmap.buffer[(face->glyph->bitmap.rows - y - 1)
                                                              * face->glyph->bitmap.width + x];
                    bitmapData[bitmapPos] = byte;
                    bitmapData[bitmapPos + 1] = byte;
                    bitmapData[bitmapPos + 2] = byte;
                    bitmapData[bitmapPos + 3] = byte;
                }
            }

            // write uvData
            const float uvOffset = 0.75f;
            uvData.push_back((float(posX) + uvOffset) / float(TEX_SIZE));
            uvData.push_back((float(posY) + uvOffset) / float(TEX_SIZE));

            uvData.push_back((float(posX + width) - uvOffset) / float(TEX_SIZE));
            uvData.push_back((float(posY) + uvOffset) / float(TEX_SIZE));

            uvData.push_back((float(posX) + uvOffset) / float(TEX_SIZE));
            uvData.push_back((float(posY + height) - uvOffset) / float(TEX_SIZE));

            uvData.push_back((float(posX + width) - uvOffset) / float(TEX_SIZE));
            uvData.push_back((float(posY + height) - uvOffset) / float(TEX_SIZE));

            // create Char
            characters[charcode] = Char{(wchar_t) charcode, texture, uvBuffer.get(),
                                        (unsigned int) (uvData.size() - 8),
                                        {((face->glyph->metrics.horiBearingX >> 6)
                                                + width/2.0f - advance/2.0f) / lineHeight,
                                         ((face->glyph->metrics.horiBearingY >> 6)
                                          + (face->size->metrics.descender >> 6)
                                          - height / 2.0f) / lineHeight},
                                        {float(width)/lineHeight, float(height)/lineHeight},
                                        float(advance)/lineHeight};

            posX += width;
        }

        charcode = FT_Get_Next_Char(face, charcode, &gindex);
    }
    FT_Done_Face(face);

    texture->set_texture(create_texture(TEX_SIZE, bitmapData));
    textures.emplace_back(texture);
    uvBuffer->buffer_data(GL_ARRAY_BUFFER, &uvData[0], uvData.size() * sizeof(GLfloat),
                          GL_STATIC_DRAW);

    isLoaded = true;
    this->size = size;
}
