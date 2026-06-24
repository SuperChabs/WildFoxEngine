#pragma once

#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/components/Components.h"
#include "ECS/World.h"
#include "core/Input.h"

class InputControllerSystem {
public:
    void Update(ECSWorld &world, Input &input, float deltaTime, bool isEnabled);

private:
    void ProcessKeyboard(Input &input, TransformComponent &transform,
                         CameraOrientationComponent &orientation, float speed, float deltaTime);

    void ProcessMouse(Input &input, TransformComponent &transform, CameraOrientationComponent &orientation,
                      float sensitivity);

    void ProcessScroll(Input &input, CameraComponent &camera);
};