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

void AudioSourceSerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    auto &audio = world->AddComponent<AudioSourceComponent>(entity);
    audio.path = data.value("path", "");
    audio.volume = data.value("volume", 1.0f);
    audio.pitch = data.value("pitch", 1.0f);
    audio.loop = data.value("loop", false);
    audio.spatical = data.value("spatical", true);
    audio.playOnStart = data.value("playOnStart", false);
}

bool AudioSourceSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<AudioSourceComponent>(entity);
}