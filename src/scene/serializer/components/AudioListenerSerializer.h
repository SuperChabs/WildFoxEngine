#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class AudioListenerSerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override {
        if (!world->HasComponent<AudioListenerComponent>(entity))
            return json::object();

        return json::object();
    }

    void Deserialize(ECSWorld *world, entt::entity entity, const json &data) override {
        world->AddComponent<AudioListenerComponent>(entity);
    }

    bool CanSerialize(ECSWorld *world, entt::entity entity) const override {
        return world->HasComponent<AudioListenerComponent>(entity);
    }
};