#pragma once

#include <string>
#include "ECS/World.h"
#include "resource/shader/ShaderManager.h"


class LightSystem {
public:
    static void Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &shaderName,
                       const std::unordered_map<entt::entity, int> *shadowMapIndices = nullptr,
                       const std::unordered_map<entt::entity, int> *pointShadowIndices = nullptr);
};