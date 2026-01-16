module;

#include <entt.hpp>
#include <glm/glm.hpp>

#include <string>

export module XEngine.ECS.Systems;

import XEngine.ECS.Components; 
import XEngine.ECS.ECSWorld;

import XEngine.Core.Camera;

import XEngine.Resource.Shader.ShaderManager; 

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
            
            if (world.HasComponent<ColorComponent>(entity)) 
            {
                auto& color = world.GetComponent<ColorComponent>(entity);
                if (meshComp.mesh) 
                    meshComp.mesh->SetColor(color.color);
            }
            
            if (meshComp.mesh) 
                meshComp.mesh->Draw(shaderManager, name);
        });

        shaderManager.Unbind();
    }
};

export class RotationSystem 
{
public:
    void Update(ECSWorld& world, float deltaTime) 
    {
        world.Each<TransformComponent, RotationComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                RotationComponent& rotation) 
        {
            if (rotation.autoRotate) 
                transform.rotation += rotation.axis * rotation.speed * deltaTime;
        });
    }
};