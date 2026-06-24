#include "CameraSystem.h"
#include "core/logging/Logger.h"

void CameraSystem::Update(ECSWorld &ecs, Input &input, float deltaTime, bool viewportFocused) {
    entt::entity activeCamera = FindMainCamera(ecs);
    if (activeCamera == entt::null) {
        Logger::Log(LogLevel::CRITICAL, LogCategory::ECS_SYSTEM, "Active camera not found!");
        return;
    }

    if (!viewportFocused) return;

    auto &camera = ecs.GetComponent<CameraComponent>(activeCamera);
    auto &transform = ecs.GetComponent<TransformComponent>(activeCamera);
    auto &orientation = ecs.GetComponent<CameraOrientationComponent>(activeCamera);
}

entt::entity CameraSystem::FindMainCamera(ECSWorld &world) {
    entt::entity result = entt::null;

    world.Each<CameraComponent, TransformComponent, CameraOrientationComponent>(
        [&](entt::entity entity, CameraComponent &cam, TransformComponent &, CameraOrientationComponent &) {
            if (!cam.isActive) return;
            if (result == entt::null || cam.isMainCamera)
                result = entity;
        });

    return result;
}

glm::vec3 CameraSystem::CalculateFront(const CameraOrientationComponent &orientation) {
    glm::vec3 front;
    front.x = cos(glm::radians(orientation.yaw)) * cos(glm::radians(orientation.pitch));
    front.y = sin(glm::radians(orientation.pitch));
    front.z = sin(glm::radians(orientation.yaw)) * cos(glm::radians(orientation.pitch));
    return glm::normalize(front);
}