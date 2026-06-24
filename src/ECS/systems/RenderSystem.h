#pragma once

#include <string>
#include <memory>

#include "ECS/components/Components.h"
#include "ECS/World.h"
#include "core/logging/Logger.h"
#include "resource/shader/ShaderManager.h"

class RenderSystem {
public:
    static void Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &name);
};