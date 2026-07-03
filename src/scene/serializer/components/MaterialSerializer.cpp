#include "MaterialSerializer.h"
#include <string>
#include <glm/glm.hpp>
#include "core/logging/Logger.h"

bool MaterialSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<MaterialComponent>(entity);
}

json MaterialSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    json data;

    if (!CanSerialize(world, entity))
        return data;

    auto &material = world->GetComponent<MaterialComponent>(entity);
    if (material.material) {
        data["name"] = material.material->GetName();
    }
    if (material.tiling != glm::vec2(1.0f, 1.0f))
        data["tiling"] = {{"x", material.tiling.x}, {"y", material.tiling.y}};

    return data;
}

entt::entity MaterialSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    std::string materialName = data.value("name", "");

    if (!dcx.materialManager) {
        Logger::Log(LogLevel::WARNING, "MaterialManager is null");
        return entity;
    }

    auto material = dcx.materialManager->GetMaterial(materialName);
    if (!material) {
        Logger::Log(LogLevel::WARNING, "Material not found in manager: " + materialName);
        return entity;
    }

    ApplyMaterial(dcx.world, entity, material, data);
    return entity;
}

void MaterialSerializer::ApplyMaterial(ECSWorld *ecs, entt::entity entity, const std::shared_ptr<Material> &material,
                                        const json &data) {
    if (ecs->HasComponent<MeshComponent>(entity)) {
        auto &matComp = ecs->AddComponent<MaterialComponent>(entity, material);
        if (data.contains("tiling")) {
            matComp.tiling.x = data["tiling"]["x"];
            matComp.tiling.y = data["tiling"]["y"];
        }
    }

    if (!ecs->HasComponent<HierarchyComponent>(entity))
        return;

    auto &hier = ecs->GetComponent<HierarchyComponent>(entity);
    for (entt::entity child : hier.children)
        ApplyMaterial(ecs, child, material, data);
}
