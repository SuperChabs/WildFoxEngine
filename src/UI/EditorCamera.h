#pragma once

#include "ECS/components/Components.h"

struct EditorCamera {
    c_Transform transform;
    c_Camera camera;
    c_CameraOrientation orientation;

    glm::vec3 pivot{0.0f, 0.0f, 0.0f};
    float distance = 10.0f;

    EditorCamera() {
        camera.isMainCamera = true;
    }
};