module;

#include <string>

#include <glad/glad.h>

export module WildFoxEngine.Resource.Shader.ShaderCompiler;

import WildFoxEngine.Core.Logger;

void CheckCompileErrors(unsigned int obj, const std::string& type);

export GLuint CompileShader(std::string vertexCode, std::string fragmentCode, std::string geometryCode)
{
    const char* vSrc = vertexCode.c_str();
    const char* fSrc = fragmentCode.c_str();

    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vSrc, nullptr);
    glCompileShader(v);
    CheckCompileErrors(v, "VERTEX");

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fSrc, nullptr);
    glCompileShader(f);
    CheckCompileErrors(f, "FRAGMENT");

    unsigned int g = 0;
    if (!geometryCode.empty())
    {
        const char* gSrc = geometryCode.c_str();
        g = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(g, 1, &gSrc, nullptr);
        glCompileShader(g);
        CheckCompileErrors(g, "GEOMETRY");
    }

    GLuint ID = glCreateProgram();
    glAttachShader(ID, v);
    glAttachShader(ID, f);
    if (!geometryCode.empty()) glAttachShader(ID, g);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    glDeleteShader(v);
    glDeleteShader(f);
    if (!geometryCode.empty()) glDeleteShader(g);

    Logger::Log(LogLevel::INFO, "Shader created with ID: " + std::to_string(ID));
    return ID;
}

void CheckCompileErrors(unsigned int obj, const std::string& type)
{
    int ok = 0;
    char log[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(obj, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            glGetShaderInfoLog(obj, 1024, nullptr, log);
            Logger::Log(LogLevel::ERROR, "SHADER COMPILE ERROR (" + type + ")\n" + log);
        }
    }
    else
    {
        glGetProgramiv(obj, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            glGetProgramInfoLog(obj, 1024, nullptr, log);
            Logger::Log(LogLevel::ERROR, "SHADER LINK ERROR\n" + std::string(log));
        }
    }
}