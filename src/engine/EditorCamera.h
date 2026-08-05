#pragma once

#include "ECS/components/Components.h"

struct EditorCamera {
    CameraComponent camera;
    CameraOrientationComponent orientation;
    TransformComponent transform;
};