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
                const std::unordered_map<entt::entity, int> *shadowMapIndices = nullptr,
                const std::unordered_map<entt::entity, int> *pointShadowIndices = nullptr);
};