#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "core/Input.h"
#include "ECS/World.h"
#include "ECS/components/Components.h"

class CameraSystem {
    entt::entity mainCameraEntity = entt::null;

public:
    CameraSystem() = default;

    void Update(ECSWorld &ecs, Input &input, float deltaTime, bool viewportFocused);

    entt::entity FindMainCamera(ECSWorld &world);

private:
    glm::vec3 CalculateFront(const CameraOrientationComponent &orientation);
};