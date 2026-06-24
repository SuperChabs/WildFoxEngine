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

void ColorSerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    if (world->HasComponent<MaterialComponent>(entity))
        return;

    glm::vec3 color = {data[0], data[1], data[2]};
    world->AddComponent<ColorComponent>(entity, color);
}

bool ColorSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<ColorComponent>(entity) &&
           !world->HasComponent<MaterialComponent>(entity);
}