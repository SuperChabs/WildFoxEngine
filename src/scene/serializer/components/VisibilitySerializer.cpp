#include "VisibilitySerializer.h"

json VisibilitySerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<VisibilityComponent>(entity))
        return json::object();

    auto &vis = world->GetComponent<VisibilityComponent>(entity);
    return json{
        {"isActive", vis.isActive},
        {"visible", vis.visible}
    };
}

void VisibilitySerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    auto &visComp = world->AddComponent<VisibilityComponent>(entity);
    visComp.isActive = data.value("isActive", true);
    visComp.visible = data.value("visible", true);
}

bool VisibilitySerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<VisibilityComponent>(entity);
}