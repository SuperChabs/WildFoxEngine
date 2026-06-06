#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class ScriptSerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override {
        if (!world->HasComponent<ScriptComponent>(entity))
            return json::object();

        auto &script = world->GetComponent<ScriptComponent>(entity);
        return json{{"path", script.scriptPath}};
    }

    void Deserialize(ECSWorld *world, entt::entity entity, const json &data) override {
        std::string scriptPath = data.value("path", "");
        auto &scriptComp = world->AddComponent<ScriptComponent>(entity);
        scriptComp.scriptPath = scriptPath;
    }

    bool CanSerialize(ECSWorld *world, entt::entity entity) const override {
        return world->HasComponent<ScriptComponent>(entity);
    }
};