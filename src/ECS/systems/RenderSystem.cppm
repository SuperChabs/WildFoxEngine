module;

#include <glad/glad.h>
#include <entt.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

export module WFE.ECS.Systems.RenderSystem;

import WFE.ECS.Components; 
import WFE.ECS.ECSWorld;

import WFE.Core.Camera;
import WFE.Core.Logger;

import WFE.Resource.Shader.ShaderManager;

export class RenderSystem 
{
public:
    void Update(ECSWorld& world, ShaderManager& shaderManager, const std::string& name, Camera& camera) 
    {
        shaderManager.Bind(name);
        
        world.Each<TransformComponent, MeshComponent, MaterialComponent, VisibilityComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                MeshComponent& meshComp,
                MaterialComponent& matComp,
                VisibilityComponent& vis) 
        {
            if (!vis.isActive || !vis.visible) return;
            
            glm::mat4 modelMatrix = world.GetGlobalTransform(entity);
            shaderManager.SetMat4(name, "model", modelMatrix);
        
            if (matComp.material)
            {
                if (meshComp.mesh) 
                    meshComp.mesh->Draw(shaderManager, name, matComp.material);
            }
            else if (world.HasComponent<ColorComponent>(entity)) 
            {
                auto& color = world.GetComponent<ColorComponent>(entity);
                if (meshComp.mesh) 
                    meshComp.mesh->SetColor(color.color);
                
                if (meshComp.mesh)
                    meshComp.mesh->Draw(shaderManager, name);
            }
            else
            {
                if (meshComp.mesh)
                    meshComp.mesh->Draw(shaderManager, name);
            }
        });

        shaderManager.Unbind();
    }
};