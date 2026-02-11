module;

#include <glad/glad.h>
#include <entt/entt.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

export module WFE.ECS.Systems.LightSystem;

import WFE.ECS.Components; 
import WFE.ECS.ECSWorld;

import WFE.Core.Logger;

import WFE.Resource.Shader.ShaderManager;

import WFE.Scene.Light;

export class LightSystem
{
public:
    void Update(ECSWorld& world, ShaderManager& shaderManager, const std::string& shaderName)
    {
        shaderManager.Bind(shaderName);
        
        int lightIndex = 0;
        const int maxLights = 8;
        
        world.Each<LightComponent, TransformComponent>(
            [&](entt::entity entity, LightComponent& light, TransformComponent& transform)
            {
                if (!light.isActive || lightIndex >= maxLights)
                    return;
                
                light.SyncWithTransform(transform);
                
                std::string base = "lights[" + std::to_string(lightIndex) + "]";
                
                shaderManager.SetInt(shaderName, base + ".type", static_cast<int>(light.type));
                
                shaderManager.SetVec3(shaderName, base + ".position", light.position);
                shaderManager.SetVec3(shaderName, base + ".direction", light.direction);
                
                shaderManager.SetVec3(shaderName, base + ".ambient", light.ambient * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".diffuse", light.diffuse * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".specular", light.specular);
                
                if (light.type == LightType::POINT || light.type == LightType::SPOT)
                {
                    shaderManager.SetFloat(shaderName, base + ".constant", light.constant);
                    shaderManager.SetFloat(shaderName, base + ".linear", light.linear);
                    shaderManager.SetFloat(shaderName, base + ".quadratic", light.quadratic);
                }
                
                if (light.type == LightType::SPOT)
                {
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