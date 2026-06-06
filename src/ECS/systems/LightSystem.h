#pragma once

#include <string>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ECS/components/Components.h"
#include "ECS/World.h"
#include "core/logging/Logger.h"
#include "resource/shader/ShaderManager.h"
#include "scene/Light.h"

class LightSystem {
public:
    void Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &shaderName,
                const std::vector<int> *shadowMapIndices = nullptr) {
        shaderManager.Bind(shaderName);

        int lightIndex = 0;
        const int maxLights = 8;

        world.Each<LightComponent, TransformComponent>(
            [&](entt::entity entity, LightComponent &light, TransformComponent &transform) {
                if (!light.isActive || lightIndex >= maxLights)
                    return;

                light.SyncWithTransform(transform);

                std::string base = "lights[" + std::to_string(lightIndex) + "]";

                // Set shadow map index
                int shadowIndex = -1;
                if (shadowMapIndices && lightIndex < shadowMapIndices->size())
                    shadowIndex = (*shadowMapIndices)[lightIndex];

                shaderManager.SetInt(shaderName, base + ".type", static_cast<int>(light.type));
                shaderManager.SetInt(shaderName, base + ".shadowIndex", shadowIndex);

                shaderManager.SetVec3(shaderName, base + ".position", light.position);
                shaderManager.SetVec3(shaderName, base + ".direction", light.direction);

                shaderManager.SetVec3(shaderName, base + ".ambient", light.ambient * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".diffuse", light.diffuse * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".specular", light.specular);

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
};