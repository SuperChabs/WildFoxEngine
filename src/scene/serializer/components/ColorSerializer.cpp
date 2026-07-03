#include "ColorSerializer.h"
#include <glm/glm.hpp>

json ColorSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<ColorComponent>(entity))
        return json::object();
    if (world->HasComponent<MaterialComponent>(entity))
        return json::object();

    auto &color = world->GetComponent<ColorComponent>(entity);
    return json{color.color.x, color.color.y, color.color.z};
}

entt::entity ColorSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    if (dcx.world->HasComponent<MaterialComponent>(entity))
        return entt::null;

    glm::vec3 color = {data[0], data[1], data[2]};
    dcx.world->AddComponent<ColorComponent>(entity, color);

    return entity;
}

bool ColorSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<ColorComponent>(entity) &&
           !world->HasComponent<MaterialComponent>(entity);
}