#pragma once

#include <imgui.h>

#include "physics/PhysicsModule.h"
#include "ECS/systems/PhysicsDebugRenderSystem.h"

class PhysicsControlPanel {
public:
    static void Render(PhysicsModule *physicsModule, PhysicsDebugRenderSystem *debugSystem);
};