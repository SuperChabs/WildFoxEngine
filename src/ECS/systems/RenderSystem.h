#pragma once

#include <string>

#include "ECS/World.h"
#include "resource/shader/ShaderManager.h"

class RenderSystem {
public:
    static void Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &name);
};