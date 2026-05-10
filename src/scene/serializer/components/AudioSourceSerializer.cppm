module;

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.Component.AudioSourceSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;

using json = nlohmann::json;

export class AudioSourceSerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<AudioSourceComponent>(entity))
            return json::object();

        auto& audio = world->GetComponent<AudioSourceComponent>(entity);
        return json{
            {"path", audio.path},
            {"volume", audio.volume},
            {"pitch", audio.pitch},
            {"loop", audio.loop},
            {"spatical", audio.spatical},
            {"playOnStart", audio.playOnStart}
        };
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        auto& audio = world->AddComponent<AudioSourceComponent>(entity);
        audio.path = data.value("path", "");
        audio.volume = data.value("volume", 1.0f);
        audio.pitch = data.value("pitch", 1.0f);
        audio.loop = data.value("loop", false);
        audio.spatical = data.value("spatical", true);
        audio.playOnStart = data.value("playOnStart", false);
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<AudioSourceComponent>(entity);
    }
};
