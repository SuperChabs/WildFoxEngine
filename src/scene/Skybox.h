#pragma once

#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "rendering/core/VertexBuffer.h"
#include "rendering/core/VertexArray.h"
#include "resource/shader/ShaderManager.h"

class Skybox {
private:
    std::unique_ptr<VertexArray> skyboxVAO;
    std::unique_ptr<VertexBuffer> skyboxVBO;

    unsigned int cubemapTexture;

    std::string shaderName;

    void SetupSkybox();

public:
    Skybox(unsigned int cubemapTex, const std::string &shaderName);


    //~Skybox() = default;

    void Render(ShaderManager &shaderManager, const glm::mat4 &view, const glm::mat4 &projection,
                bool useSkybox = true);


    void SetShader(const std::string &newShaderName);

    void SetCubemapTexture(const unsigned int newCubemap);


    std::string GetShader() const;

    unsigned int GetTexture() const;

    GLint GetVAO() const;
};