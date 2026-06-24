#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "ShaderLoader.h"
#include "ShaderConfigLoader.h"

struct ShaderObj {
    GLuint ID;
    std::string name;

    bool IsValid() const { return ID != 0; }

    bool operator ==(const ShaderObj &other) const {
        return this->name == other.name && this->ID == other.ID;
    }
};

class ShaderManager {
    ShaderConfigLoader scl;
    ShaderSource source;

    std::unordered_map<std::string, std::unique_ptr<ShaderObj> > shaders;

    std::string runtimePath = "../assets/shaders/";
    std::string sourcePath = "../assets/shaders/";

    std::string shaderConfigsFilePath = "../assets/configs/shaders.json";

    GLuint currentShader = 0;

public:
    ~ShaderManager();

    void Load();

    bool Reload(const std::string &name);

    void ReloadAll();

    void UnLoad(const std::string &name);

    void Bind(const std::string &name);

    void Unbind();

    void ClearAll();

    void SetBool(const std::string &shaderName, const std::string &variableName, bool v) const;

    void SetInt(const std::string &shaderName, const std::string &variableName, int v) const;

    void SetFloat(const std::string &shaderName, const std::string &variableName, float v) const;

    void SetVec2(const std::string &shaderName, const std::string &variableName, const glm::vec2 &v) const;

    void SetVec3(const std::string &shaderName, const std::string &variableName, const glm::vec3 &v) const;

    void SetVec4(const std::string &shaderName, const std::string &variableName, const glm::vec4 &v) const;

    void SetMat4(const std::string &shaderName, const std::string &variableName, const glm::mat4 &m) const;

    ShaderObj *GetShader(const std::string &name) const;

    GLuint GetCurrentShader();

    size_t GetCount();


    const std::unordered_map<std::string, std::unique_ptr<ShaderObj> > &GetShaderMap() const;


    bool IsShaderValid(const std::string &name) const;
};