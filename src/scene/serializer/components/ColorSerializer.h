#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class ColorSerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override {
        if (!world->HasComponent<ColorComponent>(entity))
            return json::object();
        if (world->HasComponent<MaterialComponent>(entity))
            return json::object();

        auto &color = world->GetComponent<ColorComponent>(entity);
        return json{color.color.x, color.color.y, color.color.z};
    }

    void Deserialize(ECSWorld *world, entt::entity entity, const json &data) override {
        if (world->HasComponent<MaterialComponent>(entity))
            return;

        glm::vec3 color = {data[0], data[1], data[2]};
        world->AddComponent<ColorComponent>(entity, color);
    }

    bool CanSerialize(ECSWorld *world, entt::entity entity) const override {
        return world->HasComponent<ColorComponent>(entity) &&
               !world->HasComponent<MaterialComponent>(entity);
    }
};