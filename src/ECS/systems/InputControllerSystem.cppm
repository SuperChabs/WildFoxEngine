module;

#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

export module WFE.ECS.Systems.InputControllerSystem;

import WFE.ECS.Components;
import WFE.ECS.ECSWorld;
import WFE.Core.Input;

export class InputControllerSystem
{
public:
    void Update(ECSWorld& world, Input& input, float deltaTime, bool isEnabled)
    {
        if (!isEnabled) return;
        
        world.Each<CameraComponent, TransformComponent, CameraOrientationComponent>(
            [&](entt::entity entity, CameraComponent& camera, 
                TransformComponent& transform, CameraOrientationComponent& orientation)
        {
            if (!camera.isMainCamera || !camera.isActive) return;
            
            ProcessKeyboard(input, transform, orientation, camera.movementSpeed, deltaTime);
            ProcessMouse(input, transform, orientation, camera.mouseSensitivity);
            ProcessScroll(input, camera);
        });
    }

private:
    void ProcessKeyboard(Input& input, TransformComponent& transform, 
                        CameraOrientationComponent& orientation, float speed, float deltaTime)
    {
        float velocity = speed * deltaTime;
        
        glm::vec3 front = orientation.GetFront();
        glm::vec3 right = orientation.GetRight();
        
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
            transform.position.y += velocity;
        if (input.IsKeyPressed(Key::KEY_LEFT_SHIFT))
            transform.position.y -= velocity;
    }
    
    void ProcessMouse(Input& input, TransformComponent& transform, CameraOrientationComponent& orientation, float sensitivity)
    {
        // If ImGui/ImGuizmo are using the mouse, don't update camera orientation
        if (ImGuizmo::IsUsing() || ImGui::GetIO().WantCaptureMouse)
        {
            input.ResetMouseDelta();
            return;
        }

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
};