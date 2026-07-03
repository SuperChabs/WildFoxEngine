#include "AudioListenerSerializer.h"

json AudioListenerSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<AudioListenerComponent>(entity))
        return json::object();

    return json::object();
}

entt::entity AudioListenerSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    dcx.world->AddComponent<AudioListenerComponent>(entity);

    return entity;
}

bool AudioListenerSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<AudioListenerComponent>(entity);
}