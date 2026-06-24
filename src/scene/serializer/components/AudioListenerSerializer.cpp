#include "AudioListenerSerializer.h"

json AudioListenerSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<AudioListenerComponent>(entity))
        return json::object();

    return json::object();
}

void AudioListenerSerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    world->AddComponent<AudioListenerComponent>(entity);
}

bool AudioListenerSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<AudioListenerComponent>(entity);
}