#pragma once

#include "ECS/components/Components.h"

struct EditorCamera {
    c_Camera camera;
    c_CameraOrientation orientation;
    c_Transform transform;
};