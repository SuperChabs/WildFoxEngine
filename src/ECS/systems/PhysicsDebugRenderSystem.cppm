module;

#include <memory>
#include <vector>
#include <variant>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <entt/entt.hpp>

export module WFE.ECS.Systems.PhysicsDebugRenderSystem;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Rendering.Core.VertexArray;
import WFE.Rendering.Core.VertexBuffer;
import WFE.Resource.Shader.ShaderManager;

export class PhysicsDebugRenderSystem
{
    std::unique_ptr<VertexArray> VAO;
    std::unique_ptr<VertexBuffer> VBO;

public:
    PhysicsDebugRenderSystem()
    {
        SetupBuffers();
    }

    void Update(ECSWorld& ecs, ShaderManager& shaderManager, 
                const std::string& shaderName, const glm::mat4& view, 
                const glm::mat4& projection)
    {
        shaderManager.Bind(shaderName);

        ecs.Each<TransformComponent, ColliderComponent>(
            [&](entt::entity        e,
                TransformComponent& t,
                ColliderComponent&  c)
            {
                float hue = (float)glfwGetTime() * 0.5f;
                hue -= glm::floor(hue);

                auto hsv2rgb = [](float h) -> glm::vec3 {
                    float r = glm::abs(h * 6.0f - 3.0f) - 1.0f;
                    float g = 2.0f - glm::abs(h * 6.0f - 2.0f);
                    float b = 2.0f - glm::abs(h * 6.0f - 4.0f);
                    return glm::clamp(glm::vec3(r, g, b), 0.0f, 1.0f);
                };

                shaderManager.SetMat4(shaderName, "view", view);
                shaderManager.SetMat4(shaderName, "projection", projection);
                shaderManager.SetVec3(shaderName, "color", hsv2rgb(hue));

                if (std::holds_alternative<AABB>(c.shape))
                {
                    AABB& aabb = std::get<AABB>(c.shape);

                    std::vector<glm::vec3> pts = BuildLines(aabb, t.position);

                    VBO->Bind();
                    glBufferSubData(GL_ARRAY_BUFFER, 0, pts.size() * sizeof(glm::vec3), pts.data());
                    VBO->Unbind();

                    VAO->Bind();
                    glDrawArrays(GL_LINES, 0, 24);
                    VAO->Unbind();
                }
            });

        shaderManager.Unbind();
    }

private:
    void SetupBuffers()
    {
        VAO = std::make_unique<VertexArray>();
        VBO = std::make_unique<VertexBuffer>();

        VBO->Bind();
        VAO->Bind();

        VBO->SetData(nullptr, 24 * 3 * sizeof(float), GL_DYNAMIC_DRAW);
        VAO->AddAttribute(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

        VAO->Unbind();
    }

    std::vector<glm::vec3> BuildLines(const AABB& aabb, const glm::vec3& pos)
    {
        glm::vec3 mn = aabb.min + pos;
        glm::vec3 mx = aabb.max + pos;

        glm::vec3 v[8] = 
        {
            {mn.x, mn.y, mn.z},
            {mx.x, mn.y, mn.z},
            {mx.x, mx.y, mn.z},
            {mn.x, mx.y, mn.z},
            {mn.x, mn.y, mx.z},
            {mx.x, mn.y, mx.z},
            {mx.x, mx.y, mx.z},
            {mn.x, mx.y, mx.z}
        };

        std::vector<glm::vec3> pts;

        int edges[12][2] = {
            {0,1},{1,2},{2,3},{3,0},
            {4,5},{5,6},{6,7},{7,4},
            {0,4},{1,5},{2,6},{3,7}
        };

        for (auto& e : edges)
        {
            pts.push_back(v[e[0]]);
            pts.push_back(v[e[1]]);
        }

        return pts;
    }
};