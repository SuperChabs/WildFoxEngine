#pragma once

#include <memory>
#include <vector>
#include <variant>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "rendering/core/VertexArray.h"
#include "rendering/core/VertexBuffer.h"
#include "resource/shader/ShaderManager.h"

class PhysicsDebugRenderSystem {
    std::unique_ptr<VertexArray> VAO;
    std::unique_ptr<VertexBuffer> VBO;

public:
    PhysicsDebugRenderSystem();

    void Update(ECSWorld &ecs, ShaderManager &shaderManager,
                const std::string &shaderName, const glm::mat4 &view,
                const glm::mat4 &projection);

private:
    void SetupBuffers();

    std::vector<glm::vec3> BuildLines(const AABB &aabb, const glm::vec3 &pos);
};