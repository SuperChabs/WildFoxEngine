#include "AudioSourceSerializer.h"

json AudioSourceSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<AudioSourceComponent>(entity))
        return json::object();

    auto &audio = world->GetComponent<AudioSourceComponent>(entity);
    return json{
        {"path", audio.path},
        {"volume", audio.volume},
        {"pitch", audio.pitch},
        {"loop", audio.loop},
        {"spatical", audio.spatical},
        {"playOnStart", audio.playOnStart}
    };
}

entt::entity AudioSourceSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    auto &audio = dcx.world->AddComponent<AudioSourceComponent>(entity);
    audio.path = data.value("path", "");
    audio.volume = data.value("volume", 1.0f);
    audio.pitch = data.value("pitch", 1.0f);
    audio.loop = data.value("loop", false);
    audio.spatical = data.value("spatical", true);
    audio.playOnStart = data.value("playOnStart", false);

    return entity;
}

bool AudioSourceSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<AudioSourceComponent>(entity);
}