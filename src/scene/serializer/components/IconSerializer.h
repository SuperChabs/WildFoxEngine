#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include <string>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class IconSerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override {
        if (!world->HasComponent<IconComponent>(entity))
            return json::object();

        auto &icon = world->GetComponent<IconComponent>(entity);
        return json{
            {"path", icon.iconTexturePath},
            {"scale", icon.scale},
            {"billboardMode", icon.billboardMode}
        };
    }

    void Deserialize(ECSWorld *world, entt::entity entity, const json &data) override {
        std::string path = data.value("path", "");
        float scale = data.value("scale", 0.5f);
        auto &iconComp = world->AddComponent<IconComponent>(entity, path, scale);
        iconComp.billboardMode = data.value("billboardMode", true);
    }

    bool CanSerialize(ECSWorld *world, entt::entity entity) const override {
        return world->HasComponent<IconComponent>(entity);
    }
};