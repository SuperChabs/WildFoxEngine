#include "ShaderManager.h"

#include <vector>

#include "core/logging/Logger.h"
#include "ShaderCompiler.h"

ShaderManager::~ShaderManager() { ClearAll(); }

void ShaderManager::Load() {
    const std::vector<ShaderConfig> &shaderConfigs = scl.LoadShaderConfigs(shaderConfigsFilePath);

    for (auto &config: shaderConfigs) {
        if (shaders.contains(config.name)) {
            Logger::Log(LogLevel::WARNING, "Shader already loaded, skipping: " + config.name);
            continue;
        }

        ShaderSource source = ShaderLoader::LoadShader(runtimePath + config.vertexPath,
                                                       runtimePath + config.fragmentPath,
                                                       runtimePath + config.geometryPath);
        if (source.vertex.empty() || source.fragment.empty()) {
            Logger::Log(LogLevel::ERROR,
                        "Failed to load shader source: " + config.name + " ( " + config.vertexPath + ", " + config.
                        fragmentPath + " )");
            continue;
        }

        GLuint glID = ShaderCompiler::CompileShader(source.vertex, source.fragment, source.geometry);
        if (glID == 0) {
            Logger::Log(LogLevel::ERROR, "Failed to compile shader: " + config.name);
            continue;
        }

        auto shader = std::make_unique<ShaderObj>();
        shader->ID = glID;
        shader->name = config.name;

        ShaderObj *shaderPtr = shader.get();
        shaders[config.name] = std::move(shader);

        Logger::Log(LogLevel::INFO, "Shader loaded: " + config.name + " (GL ID: " + std::to_string(glID) + ")");
    }
}

bool ShaderManager::Reload(const std::string &name) {
    std::vector<ShaderConfig> configs = scl.LoadShaderConfigs(shaderConfigsFilePath);

    auto it = shaders.find(name);
    if (it == shaders.end()) {
        Logger::Log(LogLevel::WARNING, "Cannot reload - not loaded: " + name);
        return false;
    }

    ShaderObj *oldShader = it->second.get();
    GLuint oldID = oldShader->ID;

    ShaderConfig *config = nullptr;
    for (auto &c: configs)
        if (c.name == name) {
            config = &c;
            break;
        }

    if (!config) {
        Logger::Log(LogLevel::ERROR, "Config not found for shader: " + name);
        return false;
    }

    ShaderSource newSource = ShaderLoader::LoadShader(sourcePath + config->vertexPath,
                                                      sourcePath + config->fragmentPath,
                                                      sourcePath + config->geometryPath);

    if (newSource.vertex.empty() || newSource.fragment.empty()) {
        Logger::Log(LogLevel::ERROR, "Failed to reload source: " + name);
        return false;
    }

    GLuint newID = ShaderCompiler::CompileShader(newSource.vertex, newSource.fragment, newSource.geometry);
    if (newID == 0) {
        Logger::Log(LogLevel::ERROR, "Failed to recompile: " + name);
        return false;
    }

    glDeleteProgram(oldID);

    oldShader->ID = newID;

    if (currentShader == oldID) {
        glUseProgram(newID);
        currentShader = newID;
    }

    Logger::Log(LogLevel::INFO, "Shader reloaded: " + name);
    return true;
}

void ShaderManager::ReloadAll() {
    std::vector<ShaderConfig> configs = scl.LoadShaderConfigs(shaderConfigsFilePath);

    for (auto &s: shaders) {
        ShaderObj *oldShader = s.second.get();
        GLuint oldID = oldShader->ID;

        ShaderConfig *config = nullptr;
        for (auto &c: configs)
            if (c.name == s.second->name) {
                config = &c;
                break;
            }

        if (!config) {
            Logger::Log(LogLevel::ERROR, "Config not found for shader: " + s.second->name);
            continue;
        }

        ShaderSource newSource = ShaderLoader::LoadShader(sourcePath + config->vertexPath,
                                                          sourcePath + config->fragmentPath,
                                                          sourcePath + config->geometryPath);

        if (newSource.vertex.empty() || newSource.fragment.empty()) {
            Logger::Log(LogLevel::ERROR, "Failed to reload source: " + s.second->name);
            continue;
        }

        GLuint newID = ShaderCompiler::CompileShader(newSource.vertex, newSource.fragment, newSource.geometry);
        if (newID == 0) {
            Logger::Log(LogLevel::ERROR, "Failed to recompile: " + s.second->name);
            continue;
        }

        glDeleteProgram(oldID);

        oldShader->ID = newID;

        if (currentShader == oldID) {
            glUseProgram(newID);
            currentShader = newID;
        }

        Logger::Log(LogLevel::INFO, "Shader reloaded: " + s.second->name);
    }
}

void ShaderManager::UnLoad(const std::string &name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        GLuint id = it->second->ID;

        if (currentShader == id)
            Unbind();

        if (id != 0)
            glDeleteProgram(id);

        shaders.erase(it);
        Logger::Log(LogLevel::INFO, "Shader unloaded: " + name);
    }
}

void ShaderManager::Bind(const std::string &name) {
    ShaderObj *shader = GetShader(name);
    if (shader && shader->IsValid()) {
        glUseProgram(shader->ID);
        currentShader = shader->ID;
    }
}

void ShaderManager::Unbind() {
    glUseProgram(0);
    currentShader = 0;
}

void ShaderManager::ClearAll() {
    for (auto &[name, shader]: shaders)
        if (shader->IsValid())
            glDeleteProgram(shader->ID);

    shaders.clear();
    currentShader = 0;
}

void ShaderManager::SetBool(const std::string &shaderName, const std::string &variableName, bool v) const {
    ShaderObj *shader = GetShader(shaderName);
    if (shader && shader->IsValid())
        glUniform1i(glGetUniformLocation(shader->ID, variableName.c_str()), (int) v);
}

void ShaderManager::SetInt(const std::string &shaderName, const std::string &variableName, int v) const {
    ShaderObj *shader = GetShader(shaderName);
    if (shader && shader->IsValid())
        glUniform1i(glGetUniformLocation(shader->ID, variableName.c_str()), v);
}

void ShaderManager::SetFloat(const std::string &shaderName, const std::string &variableName, float v) const {
    ShaderObj *shader = GetShader(shaderName);
    if (shader && shader->IsValid())
        glUniform1f(glGetUniformLocation(shader->ID, variableName.c_str()), v);
}

void ShaderManager::SetVec2(const std::string &shaderName, const std::string &variableName, const glm::vec2 &v) const {
    ShaderObj *shader = GetShader(shaderName);
    if (shader && shader->IsValid())
        glUniform2fv(glGetUniformLocation(shader->ID, variableName.c_str()), 1, &v[0]);
}

void ShaderManager::SetVec3(const std::string &shaderName, const std::string &variableName, const glm::vec3 &v) const {
    ShaderObj *shader = GetShader(shaderName);
    if (shader && shader->IsValid())
        glUniform3fv(glGetUniformLocation(shader->ID, variableName.c_str()), 1, &v[0]);
}

void ShaderManager::SetVec4(const std::string &shaderName, const std::string &variableName, const glm::vec4 &v) const {
    ShaderObj *shader = GetShader(shaderName);
    if (shader && shader->IsValid())
        glUniform4fv(glGetUniformLocation(shader->ID, variableName.c_str()), 1, &v[0]);
}

void ShaderManager::SetMat4(const std::string &shaderName, const std::string &variableName, const glm::mat4 &m) const {
    ShaderObj *shader = GetShader(shaderName);
    if (shader && shader->IsValid())
        glUniformMatrix4fv(
            glGetUniformLocation(shader->ID, variableName.c_str()),
            1, GL_FALSE, &m[0][0]);
}

ShaderObj *ShaderManager::GetShader(const std::string &name) const {
    auto it = shaders.find(name);
    if (it != shaders.end())
        return it->second.get();

    return nullptr;
}