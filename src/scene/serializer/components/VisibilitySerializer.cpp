#include "VisibilitySerializer.h"

json VisibilitySerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<c_Visibility>(entity))
        return json::object();

    auto &vis = world->GetComponent<c_Visibility>(entity);
    return json{
        {"isActive", vis.isActive},
        {"visible", vis.visible}
    };
}

entt::entity VisibilitySerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    auto &visComp = dcx.world->AddComponent<c_Visibility>(entity);
    visComp.isActive = data.value("isActive", true);
    visComp.visible = data.value("visible", true);

    return entity;
}

bool VisibilitySerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<c_Visibility>(entity);
}