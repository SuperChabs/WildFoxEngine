module;

#include <glad/glad.h>
#include <entt/entt.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module WFE.ECS.Systems.RotationSystem;

import WFE.ECS.Components; 
import WFE.ECS.ECSWorld;

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