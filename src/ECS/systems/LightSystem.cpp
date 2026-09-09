#include "LightSystem.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ECS/components/Components.h"

void LightSystem::Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &shaderName,
                         const std::unordered_map<entt::entity, int> *shadowMapIndices,
                         const std::unordered_map<entt::entity, int> *
                         pointShadowIndices) {
    shaderManager.Bind(shaderName);

    int lightIndex = 0;
    constexpr int maxLights = 8;

    world.Each<LightComponent, TransformComponent>(
        [&](const entt::entity entity, LightComponent &light, const TransformComponent &transform) {
            if (!light.isActive || lightIndex >= maxLights)
                return;

            light.SyncWithTransform(transform);

            const std::string base = "lights[" + std::to_string(lightIndex) + "]";

            // Set shadow map index
            int shadowIndex = -1;
            if (const auto &lookup = (light.type == LightType::POINT) ? pointShadowIndices : shadowMapIndices) {
                auto it = lookup->find(entity);
                if (it != lookup->end())
                    shadowIndex = it->second;
            }

            shaderManager.SetInt(shaderName, base + ".shadowIndex", shadowIndex);

            shaderManager.SetInt(shaderName, base + ".type", static_cast<int>(light.type));
            shaderManager.SetInt(shaderName, base + ".shadowIndex", shadowIndex);

            shaderManager.SetVec3(shaderName, base + ".position", light.position);
            shaderManager.SetVec3(shaderName, base + ".direction", light.direction);

            shaderManager.SetVec3(shaderName, base + ".ambient", light.ambient * light.intensity);
            shaderManager.SetVec3(shaderName, base + ".diffuse", light.diffuse * light.intensity);
            shaderManager.SetVec3(shaderName, base + ".specular", light.specular);

            shaderManager.SetFloat(shaderName, base + ".farPlane", std::max(light.radius, 100.0f));

            if (light.type == LightType::POINT || light.type == LightType::SPOT) {
                shaderManager.SetFloat(shaderName, base + ".constant", light.constant);
                shaderManager.SetFloat(shaderName, base + ".linear", light.linear);
                shaderManager.SetFloat(shaderName, base + ".quadratic", light.quadratic);
            }

            if (light.type == LightType::SPOT) {
                shaderManager.SetFloat(shaderName, base + ".innerCutoff",
                                       glm::cos(glm::radians(light.innerCutoff)));
                shaderManager.SetFloat(shaderName, base + ".outerCutoff",
                                       glm::cos(glm::radians(light.outerCutoff)));
            }

            lightIndex++;
        });

    shaderManager.SetInt(shaderName, "numLights", lightIndex);

    shaderManager.Unbind();
}