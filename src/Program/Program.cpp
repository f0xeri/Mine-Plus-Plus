//
// Created by Yaroslav on 28.07.2020.
//

#include "Program.hpp"
#include "GL/glew.h"
#include "GL/gl.h"
#include "../Logger.hpp"
#include <string>
#include <fstream>
#include <streambuf>

nModel::Program::Program(const std::string &vert, const std::string &frag)
{
    mProgram = glCreateProgram();
    mVertexShader = loadShader("res/glsl/" + vert + ".glsl", GL_VERTEX_SHADER);
    mFragmentShader = loadShader("res/glsl/" + frag + ".glsl", GL_FRAGMENT_SHADER);
}

nModel::Program::~Program()
{
    glDeleteProgram(mProgram);
}

GLuint nModel::Program::loadShader(const std::string &path, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    std::fstream shaderFIle(path);
    if (!shaderFIle.is_open())
    {
        LOG("[ERROR] Shader not found in: " + path);
        throw std::runtime_error("Shader file not found.");
    }
    std::string shaderStr((std::istreambuf_iterator<char>(shaderFIle)),
                    std::istreambuf_iterator<char>());

    const char *str = shaderStr.c_str();
    glShaderSource(shader, 1, &str, nullptr);

    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char errorBuf[256];
        GLsizei len;
        glGetShaderInfoLog(shader, sizeof(errorBuf), &len, errorBuf);
        errorBuf[len - 1] = ' ';
        LOG("[ERROR] Failed to compile shader " + path + ":\n\t" + errorBuf);
        throw std::runtime_error("Failed to compile shader " + path + ": " + errorBuf);
    }
    else LOG("[INFO] Shader compiled from: " + path + ".");
    return shader;
}

void nModel::Program::link()
{
    glAttachShader(mProgram, mVertexShader);
    glAttachShader(mProgram, mFragmentShader);
    glLinkProgram(mProgram);
}

void nModel::Program::bindAttribute(GLuint index, const std::string &name)
{
    glBindAttribLocation(mProgram, index, name.c_str());
}

void nModel::Program::use()
{
    glDetachShader(mProgram, mVertexShader);
    glDetachShader(mProgram, mFragmentShader);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragmentShader);
    glUseProgram(mProgram);
}

void nModel::Program::uniformMatrix(glm::mat4 matrix, const std::string& name)
{
    GLuint transformLoc = glGetUniformLocation(mProgram, name.c_str());
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}
