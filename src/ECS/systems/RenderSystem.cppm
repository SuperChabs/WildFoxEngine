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

import WFE.Rendering.Buffer;

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
            
            shaderManager.SetMat4(name, "model", transform.GetModelMatrix());
        
            if (matComp.material)
            {
                static int frameCount = 0;
                if (frameCount % 300 == 0)
                    Logger::Log(LogLevel::DEBUG, LogCategory::RENDERING, 
                        "Binding material: " + matComp.material->GetName());
                frameCount++;
                
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