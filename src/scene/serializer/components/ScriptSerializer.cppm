module;

#include <ext/entt.hpp>
#include <ext/json.hpp>

export module WFE.Scene.Serializer.Component.ScriptSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;

using json = nlohmann::json;

export class ScriptSerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<ScriptComponent>(entity))
            return json::object();

        auto& script = world->GetComponent<ScriptComponent>(entity);
        return json{{"path", script.scriptPath}};
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        std::string scriptPath = data.value("path", "");
        auto& scriptComp = world->AddComponent<ScriptComponent>(entity);
        scriptComp.scriptPath = scriptPath;
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<ScriptComponent>(entity);
    }
};