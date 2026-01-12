module;

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <memory>
#include <utility>

export module XEngine.Resource.Shader.ShaderManager;

import XEngine.Core.Logger;

import XEngine.Resource.Shader.ShaderLoader;
import XEngine.Resource.Shader.ShaderCompiler;
import XEngine.Resource.Shader.ShaderConfigLoader;

export struct ShaderObj
{
    GLuint ID;
    std::string name;

    bool IsValid() const { return ID != 0; }
};

export class ShaderManager
{
    ShaderConfigLoader scl;
    ShaderSource source;

    std::unordered_map<std::string, std::unique_ptr<ShaderObj>> shaders;

    GLuint currentShader = 0;

public:
    ~ShaderManager() { ClearAll(); }

    void Load(const std::string& path = "assets/shaders/shaders.json")
    {
        const std::vector<ShaderConfig> &shaderConfigs = scl.LoadShaderConfigs(path);

        for (auto& config : shaderConfigs)
        {
            if (shaders.contains(config.name)) 
            {
                Logger::Log(LogLevel::WARNING, "Shader already loaded, skipping: " + config.name);
                continue;
            }

            ShaderSource source = LoadShader(config.vertexPath, config.fragmentPath, config.geometryPath);
            if (source.vertex.empty() || source.fragment.empty()) 
            {
                Logger::Log(LogLevel::ERROR, 
                    "Failed to load shader source: " + config.name + " ( " + config.vertexPath + ", " + config.fragmentPath + " )");
                continue;
            }

            GLuint glID =  CompileShader(source.vertex, source.fragment, source.geometry);
            if (glID == 0) 
            {
                Logger::Log(LogLevel::ERROR, "Failed to compile shader: " + config.name);
                continue;
            }

            auto shader = std::make_unique<ShaderObj>();
            shader->ID = glID;
            shader->name = config.name;

            ShaderObj* shaderPtr = shader.get();
            shaders[config.name] = std::move(shader);

            Logger::Log(LogLevel::INFO, "Shader loaded: " + config.name + " (GL ID: " + std::to_string(glID) + ")");
        }
    }

    bool Reload(const std::string& name)
    {
        auto it = shaders.find(name);
        if (it == shaders.end()) 
        {
            Logger::Log(LogLevel::WARNING, "Cannot reload - not loaded: " + name);
            return false;
        }

        ShaderObj* oldShader = it->second.get();
        GLuint oldID = oldShader->ID;

        std::vector<ShaderConfig> configs = scl.LoadShaderConfigs("assets/shaders/shaders.json");
        
        ShaderConfig* config = nullptr;
        for (auto& c : configs) 
        {
            if (c.name == name) 
            {
                config = &c;
                break;
            }
        }

        if (!config) 
        {
            Logger::Log(LogLevel::ERROR, "Config not found for shader: " + name);
            return false;
        }

        ShaderSource newSource = LoadShader(config->vertexPath, config->fragmentPath, config->geometryPath);

        if (newSource.vertex.empty() || newSource.fragment.empty()) {
            Logger::Log(LogLevel::ERROR, "Failed to reload source: " + name);
            return false;
        }

        GLuint newID = CompileShader(newSource.vertex, newSource.fragment, newSource.geometry);
        if (newID == 0) 
        {
            Logger::Log(LogLevel::ERROR, "Failed to recompile: " + name);
            return false;
        }

        glDeleteProgram(oldID);

        oldShader->ID = newID;

        if (currentShader == oldID) 
        {
            glUseProgram(newID);
            currentShader = newID;
        }

        Logger::Log(LogLevel::INFO, "Shader reloaded: " + name);
        return true;
    }

    void UnLoad(const std::string& name)
    {
        auto it = shaders.find(name);
        if (it != shaders.end())
        {
            GLuint id = it->second->ID;

            if (currentShader == id)
                Unbind();

            if (id != 0)
                glDeleteProgram(id);

            shaders.erase(it);
            Logger::Log(LogLevel::INFO, "Shader unloaded: " + name);
        }
    }

    void Bind(const std::string& name) 
    {
        ShaderObj* shader = GetShader(name);
        if (shader && shader->IsValid())
        {
            glUseProgram(shader->ID);
            currentShader = shader->ID;
        }
    }

    void Unbind() 
    {
        glUseProgram(0);
        currentShader = 0;
    }

    void ClearAll()
    {
        for (auto& [name, shader] : shaders)
            if (shader->IsValid())
                glDeleteProgram(shader->ID);

        shaders.clear();
        currentShader = 0;

        Logger::Log(LogLevel::INFO, "All shaders unloaded");
    }

    void SetBool(const std::string& shaderName, const std::string& variableName, bool v) const
    {
        ShaderObj* shader = GetShader(shaderName);
        if (shader && shader->IsValid())
            glUniform1i(glGetUniformLocation(shader->ID, variableName.c_str()), (int)v);
    }

    void SetInt(const std::string& shaderName, const std::string& variableName, int v) const
    {
        ShaderObj* shader = GetShader(shaderName);
        if (shader && shader->IsValid())
            glUniform1i(glGetUniformLocation(shader->ID, variableName.c_str()), v);
    }

    void SetFloat(const std::string& shaderName, const std::string& variableName, float v) const
    {
        ShaderObj* shader = GetShader(shaderName);
        if (shader && shader->IsValid())
            glUniform1f(glGetUniformLocation(shader->ID, variableName.c_str()), v);
    }

    void SetVec2(const std::string& shaderName, const std::string& variableName, const glm::vec2& v) const
    {
        ShaderObj* shader = GetShader(shaderName);
        if (shader && shader->IsValid())
            glUniform2fv(glGetUniformLocation(shader->ID, variableName.c_str()), 1, &v[0]);
    }

    void SetVec3(const std::string& shaderName, const std::string& variableName, const glm::vec3& v) const
    {
        ShaderObj* shader = GetShader(shaderName);
        if (shader && shader->IsValid())
            glUniform3fv(glGetUniformLocation(shader->ID, variableName.c_str()), 1, &v[0]);
    }

    void SetVec4(const std::string& shaderName, const std::string& variableName, const glm::vec4& v) const
    {
        ShaderObj* shader = GetShader(shaderName);
        if (shader && shader->IsValid())
            glUniform4fv(glGetUniformLocation(shader->ID, variableName.c_str()), 1, &v[0]);
    }

    void SetMat4(const std::string& shaderName, const std::string& variableName, const glm::mat4& m) const
    {
        ShaderObj* shader = GetShader(shaderName);
        if (shader && shader->IsValid())
            glUniformMatrix4fv(
            glGetUniformLocation(shader->ID, variableName.c_str()),
            1, GL_FALSE, &m[0][0]);
    }

    ShaderObj* GetShader(const std::string& name) const
    {
        auto it = shaders.find(name);
        if (it != shaders.end())
            return it->second.get();

        return nullptr;
    }

    GLuint GetCurrentShader()
    {
        return currentShader;
    }

    size_t GetCount()
    {
        return shaders.size();
    }
};