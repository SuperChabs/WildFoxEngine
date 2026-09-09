#pragma once

#include <unordered_map>

#include <entt/entt.hpp>

#include "ECS/World.h"
#include "resource/material/MaterialManager.h"
#include "resource/model/ModelManager.h"

struct DeserializeContext {
    ECSWorld *world;
    ModelManager *modelManager;
    MaterialManager *materialManager;
    std::unordered_map<uint64_t, entt::entity> *id;
};