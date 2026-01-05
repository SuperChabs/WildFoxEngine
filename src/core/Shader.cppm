module; 

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>

export module XEngine.Core.Shader;

import XEngine.Utils.Logger;

export class Shader
{
public:
    unsigned int ID = 0;

    Shader(const char* vertexPath,
           const char* fragmentPath,
           const char* geometryPath = nullptr)
    {
        CompileShader(vertexPath, fragmentPath, geometryPath);
    }

    Shader(const char* shaderName, bool isGeometry = false)
    {
        const std::string name(shaderName);

        const std::string vs = "assets/shaders/vertex/"   + name + ".vs";
        const std::string fs = "assets/shaders/fragment/" + name + ".fs";

        if (isGeometry)
        {
            const std::string gs =
                "assets/shaders/geometry/" + name + ".gs";
            CompileShader(vs.c_str(), fs.c_str(), gs.c_str());
        }
        else
        {
            CompileShader(vs.c_str(), fs.c_str(), nullptr);
        }
    }

    void use() const
    {
        glUseProgram(ID);
    }

    // ───── uniforms ─────

    void setBool(const std::string& name, bool v) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)v);
    }

    void setInt(const std::string& name, int v) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), v);
    }

    void setFloat(const std::string& name, float v) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), v);
    }

    void setVec2(const std::string& name, const glm::vec2& v) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
    }

    void setVec3(const std::string& name, const glm::vec3& v) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
    }

    void setVec4(const std::string& name, const glm::vec4& v) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
    }

    void setMat4(const std::string& name, const glm::mat4& m) const
    {
        glUniformMatrix4fv(
            glGetUniformLocation(ID, name.c_str()),
            1, GL_FALSE, &m[0][0]);
    }

private:
    void checkCompileErrors(unsigned int obj,
                            const std::string& type)
    {
        int ok = 0;
        char log[1024];

        if (type != "PROGRAM")
        {
            glGetShaderiv(obj, GL_COMPILE_STATUS, &ok);
            if (!ok)
            {
                glGetShaderInfoLog(obj, 1024, nullptr, log);
                Logger::Log(
                    LogLevel::ERROR,
                    "SHADER COMPILE ERROR (" + type + ")\n" + log);
            }
        }
        else
        {
            glGetProgramiv(obj, GL_LINK_STATUS, &ok);
            if (!ok)
            {
                glGetProgramInfoLog(obj, 1024, nullptr, log);
                Logger::Log(
                    LogLevel::ERROR,
                    "SHADER LINK ERROR\n" + std::string(log));
            }
        }
    }

    void CompileShader(const char* vs,
                       const char* fs,
                       const char* gs)
    {
        std::string vCode, fCode, gCode;

        try
        {
            std::ifstream vFile(vs);
            std::ifstream fFile(fs);

            std::stringstream vStream, fStream;
            vStream << vFile.rdbuf();
            fStream << fFile.rdbuf();

            vCode = vStream.str();
            fCode = fStream.str();

            if (gs)
            {
                std::ifstream gFile(gs);
                std::stringstream gStream;
                gStream << gFile.rdbuf();
                gCode = gStream.str();
            }
        }
        catch (...)
        {
            Logger::Log(LogLevel::ERROR,
                        "Shader file read failed");
            ID = 0;
            return;
        }

        const char* vSrc = vCode.c_str();
        const char* fSrc = fCode.c_str();

        unsigned int v = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(v, 1, &vSrc, nullptr);
        glCompileShader(v);
        checkCompileErrors(v, "VERTEX");

        unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(f, 1, &fSrc, nullptr);
        glCompileShader(f);
        checkCompileErrors(f, "FRAGMENT");

        unsigned int g = 0;
        if (gs)
        {
            const char* gSrc = gCode.c_str();
            g = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(g, 1, &gSrc, nullptr);
            glCompileShader(g);
            checkCompileErrors(g, "GEOMETRY");
        }

        ID = glCreateProgram();
        glAttachShader(ID, v);
        glAttachShader(ID, f);
        if (gs) glAttachShader(ID, g);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(v);
        glDeleteShader(f);
        if (gs) glDeleteShader(g);

        Logger::Log(LogLevel::INFO,
                    "Shader created, ID=" + std::to_string(ID));
    }
};
