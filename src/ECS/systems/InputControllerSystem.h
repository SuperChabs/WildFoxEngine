#pragma once

#include "ECS/components/Components.h"

#include "core/Input.h"

class InputControllerSystem {
public:
    static void Update(c_Camera &camera, c_Transform &trans,
                c_CameraOrientation &orientation, Input &input, float deltaTime, bool isEnabled);

private:
    static void ProcessKeyboard(const Input &input, c_Transform &transform,
                                const c_CameraOrientation &orientation, float speed, float deltaTime);

    static void ProcessMouse(Input &input, c_CameraOrientation &orientation,
                             float sensitivity);

    static void ProcessScroll(Input &input, c_Camera &camera);
};