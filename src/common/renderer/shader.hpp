#ifndef RENDERER_SHADER_HPP
#define RENDERER_SHADER_HPP

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>
#include <iostream>

class Shader
{
public:

    template<class ...Args>
    Shader(Args... args)
        :shader(load_shaders(args...))
    {

    }

    ~Shader();

    Shader& operator=(const Shader&) = delete;

    void use();

    GLuint get_uniform_location(const std::string& name);

private:

    GLuint load_shader(const std::string& filePath, GLenum shaderType);
    bool check_link_shader_program_error(GLuint program);

    GLuint create_shader_program(const std::vector<GLuint>& shaders);

    template<class ...Args>
    GLuint load_shaders(const std::string& filePath, GLenum shaderType,
                        Args... args)
    {
        std::vector<GLuint> shaders;
        load_shaders(shaders, filePath, shaderType, args...);
        if(shaders.size() != 0)
        {
            return create_shader_program(shaders);
        }
        else
        {
            return 0;
        }
    }

    template<class ...Args>
    void load_shaders(std::vector<GLuint>& shaders,
                      const std::string& filePath, GLenum shaderType,
                      Args... args)
    {
        GLuint shader = load_shader(filePath, shaderType);
        if(shader == 0)
        {
            for(GLuint shader : shaders)
            {
                glDeleteShader(shader);
            }
        }
        else
        {
            shaders.push_back(shader);
            load_shaders(shaders, args...);
        }
    }

    void load_shaders(std::vector<GLuint>& shaders,
                      const std::string& filePath, GLenum shaderType)
    {
        GLuint shader = load_shader(filePath, shaderType);
        if(shader == 0)
        {
            for(GLuint shader : shaders)
            {
                glDeleteShader(shader);
            }
        }
        else
        {
            shaders.push_back(shader);
        }
    }

    GLuint shader;

};

#endif
