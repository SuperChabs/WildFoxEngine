module;

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.Component.VisibilitySerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;

using json = nlohmann::json;

export class VisibilitySerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<VisibilityComponent>(entity))
            return json::object();

        auto& vis = world->GetComponent<VisibilityComponent>(entity);
        return json{
            {"isActive", vis.isActive},
            {"visible", vis.visible}
        };
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        auto& visComp = world->AddComponent<VisibilityComponent>(entity);
        visComp.isActive = data.value("isActive", true);
        visComp.visible = data.value("visible", true);
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<VisibilityComponent>(entity);
    }
};