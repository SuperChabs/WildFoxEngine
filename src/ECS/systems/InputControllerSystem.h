#pragma once

#include "ECS/components/Components.h"

#include "core/Input.h"

class InputControllerSystem {
public:
    static void Update(CameraComponent &camera, TransformComponent &trans,
                CameraOrientationComponent &orientation, Input &input, float deltaTime, bool isEnabled);

private:
    static void ProcessKeyboard(const Input &input, TransformComponent &transform,
                                const CameraOrientationComponent &orientation, float speed, float deltaTime);

    static void ProcessMouse(Input &input, CameraOrientationComponent &orientation,
                             float sensitivity);

    static void ProcessScroll(Input &input, CameraComponent &camera);
};