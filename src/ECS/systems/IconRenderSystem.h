#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glad/glad.h>

#include "ECS/components/Components.h"
#include "ECS/World.h"
#include "resource/shader/ShaderManager.h"
#include "resource/texture/TextureManager.h"
#include "rendering/core/VertexArray.h"
#include "rendering/core/VertexBuffer.h"

class IconRenderSystem {
private:
    std::unique_ptr<VertexArray> quadVAO;
    std::unique_ptr<VertexBuffer> quadVBO;
    TextureManager *textureManager;

    void SetupQuad();

public:
    IconRenderSystem(TextureManager *tm);

    void Update(ECSWorld &world, ShaderManager &shaderManager,
                const std::string &shaderName, const glm::mat4 &view,
                const glm::mat4 &projection);
};