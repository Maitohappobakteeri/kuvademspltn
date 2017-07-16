#include "texture.hpp"

#include <png.h>

#include <iostream>
#include <fstream>
#include <vector>


namespace
{
    void ifstreamPngRead(png_structp pngPtr, png_bytep out, png_size_t count)
    {
        png_voidp ioPtr = png_get_io_ptr(pngPtr);
        if(ioPtr == 0)
        {
            return;
        }
        std::ifstream &file = *((std::ifstream*) ioPtr);
        file.read((char*) out, count);
    }
};


Texture::Texture(Texture* placeholder)
    :filePath(""), texture(0), width(0), height(0), isLoaded(false)
{
    if(placeholder != nullptr)
    {
        texture = placeholder->texture;
        width = placeholder->width;
        height = placeholder->height;
    }
}


Texture::Texture(GLuint ID)
    :filePath(""), texture(0), width(0), height(0), isLoaded(false)
{
    set_texture(ID);
}


Texture::Texture(const std::string& filePath, bool loadNow, Texture* placeholder)
    :filePath(filePath), texture(0), width(0), height(0), isLoaded(false)
{
    if(loadNow)
    {
        texture = load_texture(filePath);
        if(isLoaded != 0)
        {
            isLoaded = true;
        }
    }

    if(!isLoaded && placeholder != nullptr)
    {
        texture = placeholder->texture;
        width = placeholder->width;
        height = placeholder->height;
    }
}


Texture::Texture(Texture&& l)
    :filePath(l.filePath), texture(l.texture), width(l.width), height(l.height),
     isLoaded(l.isLoaded)
{
    l.isLoaded = false;
}


Texture::~Texture()
{
    if(isLoaded)
    {
        glDeleteTextures(1, &texture);
    }
}


Texture Texture::create_empty(unsigned int width, unsigned height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return Texture(texture);
}


void Texture::set_texture(GLuint ID)
{
    if(isLoaded)
    {
        glDeleteTextures(1, &texture);
        isLoaded = false;
    }

    texture = ID;
    isLoaded = true;

    int w,h;

    bind();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

    width = w;
    height = h;
}


GLuint Texture::get_texture() const
{
    if(isLoaded)
    {
        return texture;
    }
    else
    {
        return 0;
    }
}


void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, texture);
}


bool Texture::load(Texture* placeholder)
{
    texture = load_texture(filePath);

    if(!is_loaded() && placeholder != nullptr)
    {
        texture = placeholder->texture;
        width = placeholder->width;
        height = placeholder->height;
    }
    return is_loaded();
}


GLuint Texture::load_texture(const std::string& filePath)
{
    if(isLoaded)
    {
        glDeleteTextures(1, &texture);
        isLoaded = false;
    }
    texture = 0;
    width = 0;
    height = 0;

    const GLuint INVALID_TEXTURE = 0;

    std::cout << "loading texture: " << filePath << std::endl;

    // open texture file
    std::ifstream textureFile(filePath, std::ifstream::binary);
    if(textureFile.fail())
    {
        std::cerr << "failed to open file" << std::endl;
        return INVALID_TEXTURE;
    }

    // read header
    png_byte header[8];
    textureFile.read((char*) header, 8);

    if(png_sig_cmp(header, 0, 8))
    {
        std::cerr << "file is not a png" << std::endl;
        textureFile.close();
        return INVALID_TEXTURE;
    }

    // create read struct
    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!pngPtr)
    {
        std::cerr << "couldn't create png read struct" << std::endl;
        textureFile.close();
        return INVALID_TEXTURE;
    }

    // create info struct
    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr)
    {
        std::cerr << "couldn't create png info struct" << std::endl;
        png_destroy_read_struct(&pngPtr, (png_infopp)nullptr, (png_infopp)nullptr);
        textureFile.close();
        return INVALID_TEXTURE;
    }

    // create end info struct
    png_infop endPtr = png_create_info_struct(pngPtr);
    if (!endPtr)
    {
        std::cerr << "couldn't create png info struct" << std::endl;
        png_destroy_read_struct(&pngPtr, &infoPtr, (png_infopp) NULL);
        textureFile.close();
        return INVALID_TEXTURE;
    }

    // check for error
    if (setjmp(png_jmpbuf(pngPtr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&pngPtr, &infoPtr, &endPtr);
        textureFile.close();
        return INVALID_TEXTURE;
    }

    png_set_read_fn(pngPtr, (void*) &textureFile, ifstreamPngRead);
    png_set_sig_bytes(pngPtr, 8);

    // read all data
    png_read_info(pngPtr, infoPtr);

    int bitDepth, colorType;
    png_uint_32 tmpWidth, tmpHeight;
    png_get_IHDR(pngPtr, infoPtr, &tmpWidth, &tmpHeight, &bitDepth, &colorType,
                 nullptr, nullptr, nullptr);
    png_read_update_info(pngPtr, infoPtr);

    int rowBytes = png_get_rowbytes(pngPtr, infoPtr);
    // make 4byte aligned
    rowBytes += 3 - ((rowBytes-1) % 4);
    std::vector<png_byte> imageData(rowBytes * tmpHeight);
    std::vector<png_bytep> rowPointers(tmpHeight);

    for (unsigned int i = 0; i < tmpHeight; i++)
    {
        rowPointers[tmpHeight - 1 - i] = &imageData[i * rowBytes];
    }
    png_read_image(pngPtr, &rowPointers[0]);

    // create gl texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tmpWidth, tmpHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, &imageData[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clean up
    png_destroy_read_struct(&pngPtr, &infoPtr, &endPtr);

    textureFile.close();

    width = tmpWidth;
    height = tmpHeight;

    isLoaded = true;
    return texture;
}


std::string Texture::get_path() const
{
    return filePath;
}


unsigned int Texture::get_width() const
{
    return width;
}


unsigned int Texture::get_height() const
{
    return height;
}


bool Texture::is_loaded() const
{
    return isLoaded;
}
