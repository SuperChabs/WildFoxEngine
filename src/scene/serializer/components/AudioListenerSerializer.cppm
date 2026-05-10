module;

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.Component.AudioListenerSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;

using json = nlohmann::json;

export class AudioListenerSerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<AudioListenerComponent>(entity))
            return json::object();

        return json::object();
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        world->AddComponent<AudioListenerComponent>(entity);
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<AudioListenerComponent>(entity);
    }
};
