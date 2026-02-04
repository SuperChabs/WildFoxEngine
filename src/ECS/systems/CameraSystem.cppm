module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <entt/entt.hpp>
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"

export module WFE.ECS.Systems.CameraSystem;

import WFE.Core.Camera;
import WFE.Core.Input;
import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Logger;

export class CameraSystem
{
    entt::entity mainCameraEntity = entt::null;

    Camera* legacyCamera = nullptr;

public:
    CameraSystem() = default;

    void SetLagacyCamera(Camera* cam) { legacyCamera = cam; }

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

        ProcessKeyboard(input, transform, orientation, camera.movementSpeed, deltaTime);
        ProcessMouse(input, orientation, camera.mouseSensitivity);
        ProcessScroll(input, camera);
        
        SyncToLegacyCamera(transform, orientation, camera);
    }

private:
    entt::entity FindMainCamera(ECSWorld& world)
    {
        entt::entity result = entt::null;
        
        world.Each<CameraComponent, TransformComponent, CameraOrientationComponent>(
            [&](entt::entity entity, CameraComponent& cam, TransformComponent&, CameraOrientationComponent&)
        {
            if (cam.isMainCamera && cam.isActive)
                result = entity;
        });
        
        return result;
    }
    
    void ProcessKeyboard(Input& input, TransformComponent& transform, 
                        CameraOrientationComponent& orientation, float speed, float deltaTime)
    {
        float velocity = speed * deltaTime;
        
        glm::vec3 front = CalculateFront(orientation);
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
        
        if (input.IsKeyPressed(Key::KEY_W))
        {
            glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
            transform.position += flatFront * velocity;
        }
        if (input.IsKeyPressed(Key::KEY_S))
        {
            glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
            transform.position -= flatFront * velocity;
        }
        
        if (input.IsKeyPressed(Key::KEY_A))
            transform.position -= right * velocity;
        if (input.IsKeyPressed(Key::KEY_D))
            transform.position += right * velocity;
        
        if (input.IsKeyPressed(Key::KEY_SPACE))
            transform.position += glm::vec3(0, 1, 0) * velocity;
        if (input.IsKeyPressed(Key::KEY_LEFT_SHIFT))
            transform.position -= glm::vec3(0, 1, 0) * velocity;
    }
    
    void ProcessMouse(Input& input, CameraOrientationComponent& orientation, float sensitivity)
    {
        if (!input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
        {
            input.ResetMouseDelta();
            return;
        }
        
        glm::vec2 delta = input.GetMouseDelta();
        
        orientation.yaw += delta.x * sensitivity;
        orientation.pitch += delta.y * sensitivity;
        
        if (orientation.pitch > 89.0f) orientation.pitch = 89.0f;
        if (orientation.pitch < -89.0f) orientation.pitch = -89.0f;
        
        if (orientation.yaw > 360.0f || orientation.yaw < -360.0f)
            orientation.yaw = 0.0f;
    }
    
    void ProcessScroll(Input& input, CameraComponent& camera)
    {
        float scrollOffset = input.GetScrollOffset();
        
        if (scrollOffset != 0.0f)
        {
            camera.fov -= scrollOffset;
            
            if (camera.fov < 1.0f) camera.fov = 1.0f;
            if (camera.fov > 55.0f) camera.fov = 55.0f;
        }
        
        input.ResetScrollOffset();
    }

    glm::vec3 CalculateFront(const CameraOrientationComponent& orientation)
    {
        glm::vec3 front;
        front.x = cos(glm::radians(orientation.yaw)) * cos(glm::radians(orientation.pitch));
        front.y = sin(glm::radians(orientation.pitch));
        front.z = sin(glm::radians(orientation.yaw)) * cos(glm::radians(orientation.pitch));
        return glm::normalize(front);
    }
    
    void SyncToLegacyCamera(const TransformComponent& transform, 
                           const CameraOrientationComponent& orientation,
                           const CameraComponent& camera)
    {
        if (!legacyCamera) return;
        
        legacyCamera->SetPosition(transform.position);
        legacyCamera->SetYaw(orientation.yaw);
        legacyCamera->SetPitch(orientation.pitch);
        legacyCamera->SetZoom(camera.fov);
        legacyCamera->SetMovementSpeed(camera.movementSpeed);
        legacyCamera->SetMouseSensitivity(camera.mouseSensitivity);
    }
};