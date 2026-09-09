#include "InputControllerSystem.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>

void InputControllerSystem::Update(CameraComponent &camera, TransformComponent &trans,
        CameraOrientationComponent &orientation, Input &input, const float deltaTime, const bool isEnabled) {
    if (!isEnabled) return;

    ProcessKeyboard(input, trans, orientation, camera.movementSpeed, deltaTime);
    ProcessMouse(input, orientation, camera.mouseSensitivity);
    ProcessScroll(input, camera);
}

void InputControllerSystem::ProcessKeyboard(const Input &input, TransformComponent &transform,
        const CameraOrientationComponent &orientation, const float speed, const float deltaTime) {
    const float velocity = speed * deltaTime;

    const glm::vec3 front = orientation.GetFront();
    const glm::vec3 right = orientation.GetRight();

    if (input.IsKeyPressed(Key::KEY_W)) {
        const glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        transform.position += flatFront * velocity;
    }
    if (input.IsKeyPressed(Key::KEY_S)) {
        const glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
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

void InputControllerSystem::ProcessMouse(Input &input, CameraOrientationComponent &orientation, const float sensitivity) {
    // If ImGui/ImGuizmo are using the mouse, don't update camera orientation
    if (ImGuizmo::IsUsing() || ImGui::GetIO().WantCaptureMouse) {
        input.ResetMouseDelta();
        return;
    }

    if (!input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
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

void InputControllerSystem::ProcessScroll(Input &input, CameraComponent &camera) {
    if (const double scrollOffset = Input::GetScrollOffset(); scrollOffset != 0.0f) {
        camera.fov -= static_cast<float>(scrollOffset);

        if (camera.fov < 1.0f) camera.fov = 1.0f;
        if (camera.fov > 55.0f) camera.fov = 55.0f;
    }

    Input::ResetScrollOffset();
}