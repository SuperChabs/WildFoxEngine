module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <entt/entt.hpp>
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"

export module WFE.ECS.Systems.CameraSystem;

import WFE.Core.Input;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;

export class CameraSystem
{
    entt::entity mainCameraEntity = entt::null;


public:
    CameraSystem() = default;

    void Update(ECSWorld& ecs, Input& input, float deltaTime, bool viewportFocused)
    {
        entt::entity activeCamera = FindMainCamera(ecs);
        if (activeCamera == entt::null)
        {
            Logger::Log(LogLevel::CRITICAL, LogCategory::ECS_SYSTEM, "Active camera not found!");
            return;
        }

        if (!viewportFocused) return;

        auto& camera = ecs.GetComponent<CameraComponent>(activeCamera);
        auto& transform = ecs.GetComponent<TransformComponent>(activeCamera);
        auto& orientation = ecs.GetComponent<CameraOrientationComponent>(activeCamera);
    }

    entt::entity FindMainCamera(ECSWorld& world)
    {
        entt::entity result = entt::null;
        
        world.Each<CameraComponent, TransformComponent, CameraOrientationComponent>(
            [&](entt::entity entity, CameraComponent& cam, TransformComponent&, CameraOrientationComponent&)
        {
            if (cam.isMainCamera && cam.isActive)
                result = entity;
            else if (cam.isActive)
                result = entity;
            else
                result = entt::null;
        });
        
        return result;
    }

private:
    glm::vec3 CalculateFront(const CameraOrientationComponent& orientation)
    {
        glm::vec3 front;
        front.x = cos(glm::radians(orientation.yaw)) * cos(glm::radians(orientation.pitch));
        front.y = sin(glm::radians(orientation.pitch));
        front.z = sin(glm::radians(orientation.yaw)) * cos(glm::radians(orientation.pitch));
        return glm::normalize(front);
    }
};