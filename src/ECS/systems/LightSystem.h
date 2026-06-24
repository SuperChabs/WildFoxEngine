#pragma once

#include <string>
#include <vector>

#include "ECS/components/Components.h"
#include "ECS/World.h"
#include "core/logging/Logger.h"
#include "resource/shader/ShaderManager.h"
#include "scene/Light.h"

class LightSystem {
public:
    void Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &shaderName,
                const std::vector<int> *shadowMapIndices = nullptr);
};