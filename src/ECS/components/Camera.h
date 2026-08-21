#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraComponent {
    float fov = 55.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    float movementSpeed = 3.5f;
    float mouseSensitivity = 0.1f;

    bool isMainCamera = false;
    bool isActive = true;

    CameraComponent() = default;

    [[nodiscard]] glm::mat4 GetProjectionMatrix(const float aspectRatio) const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
};

struct CameraOrientationComponent {
    float yaw = -95.0f;
    float pitch = 0.0f;

    CameraOrientationComponent() = default;

    [[nodiscard]] glm::vec3 GetFront() const {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return glm::normalize(front);
    }

    [[nodiscard]] glm::vec3 GetRight() const {
        const glm::vec3 front = GetFront();
        return glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    }

    [[nodiscard]] glm::vec3 GetUp() const {
        const glm::vec3 front = GetFront();
        const glm::vec3 right = GetRight();
        return glm::normalize(glm::cross(right, front));
    }

    [[nodiscard]] glm::mat4 GetViewMatrix(const glm::vec3 &position) const {
        const glm::vec3 front = GetFront();
        return glm::lookAt(position, position + front, GetUp());
    }
};