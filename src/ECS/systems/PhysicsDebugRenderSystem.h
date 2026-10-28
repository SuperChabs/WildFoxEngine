#pragma once

#include <memory>
#include <vector>
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
    bool m_enabled = true;

    PhysicsDebugRenderSystem();

    void Update(ECSWorld &ecs, ShaderManager &shaderManager,
                const std::string &shaderName, const glm::mat4 &view,
                const glm::mat4 &projection) const;

    struct DebugMesh {
        std::vector<glm::vec3> tris, lines, points;
    };

private:
    void SetupBuffers();
    void Draw(GLenum mode, const std::vector<glm::vec3> &pts) const;

    static void BuildSphereMesh(const Sphere &sphere, const glm::vec3 &pos, DebugMesh &mesh, int segments = 16, int rings = 12);
    static std::vector<glm::vec3> BuildLines(const AABB &aabb, const glm::vec3 &pos);
    static std::vector<glm::vec3> BuildSphereLines(const Sphere &sphere, const glm::vec3 &pos, int segments = 24);
    static void BuildHullMesh(const ConvexHull &hull, const glm::vec3 &pos, const glm::quat &rot,
                              const glm::vec3 &scale, DebugMesh &mesh);
};