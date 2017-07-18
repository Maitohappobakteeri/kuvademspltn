#include "shader.hpp"


#include "print.hpp"

#include <fstream>
#include <iostream>


Shader::~Shader()
{
    glDeleteShader(shader);
}


void Shader::use()
{
    glUseProgram(shader);
}


GLuint Shader::get_uniform_location(const std::string& name)
{
    return glGetUniformLocation(shader, name.c_str());
}


GLuint Shader::load_shader(const std::string& filePath, GLenum shaderType)
{
    println("loading shader: ", filePath);

    std::ifstream file(filePath);
    if(file.fail())
    {
        std::cerr << "failed to open file" << std::endl;
        return 0;
    }

    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);

    GLchar* buffer = new GLchar[length];
    file.read(buffer, length);

    if(file.fail())
    {
        std::cerr << "failed to read file" << std::endl;
        return 0;
    }

    file.close();

    GLuint shader = glCreateShader(shaderType);
    if(shader == 0)
    {
        std::cerr << "failed to create shader" << std::endl;
        return 0;
    }

    println("compiling shader: ", filePath);

    glShaderSource(shader, 1, &buffer, &length);
    glCompileShader(shader);
    delete[] buffer;

    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(!result)
    {
        GLint infoLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
        GLchar* infoBuffer = new GLchar[infoLength];

        glGetShaderInfoLog(shader, infoLength, NULL, infoBuffer);
        std::cerr << "failed to compile shader: " << infoBuffer << std::endl;
        delete[] infoBuffer;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


bool Shader::check_link_shader_program_error(GLuint program)
{
    GLint result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if(!result)
    {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar* buffer = new GLchar[length];
        glGetProgramInfoLog(program, length, NULL, buffer);
        std::cerr << "failed to link shaders: " << buffer << std::endl;
        delete[] buffer;
        return true;
    }
    return false;
}


GLuint Shader::create_shader_program(const std::vector<GLuint>& shaders)
{
    println("linking shader program");

    GLuint program = glCreateProgram();

    for(GLuint shader : shaders)
    {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);
    for(GLuint shader : shaders)
    {
        glDeleteShader(shader);
    }

    if(check_link_shader_program_error(program))
    {
        glDeleteProgram(program);
        program = 0;
    }
    return program;
}
