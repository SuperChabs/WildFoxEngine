#pragma once

#include <string>
#include <glad/glad.h>

namespace ShaderCompiler {
    GLuint CompileShader(std::string vertexCode, std::string fragmentCode, std::string geometryCode);

    void CheckCompileErrors(unsigned int obj, const std::string &type);
} // namespace ShaderCompiler