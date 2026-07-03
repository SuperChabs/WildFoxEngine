#include "IconSerializer.h"
#include <string>

json IconSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<IconComponent>(entity))
        return json::object();

    auto &icon = world->GetComponent<IconComponent>(entity);
    return json{
        {"path", icon.iconTexturePath},
        {"scale", icon.scale},
        {"billboardMode", icon.billboardMode}
    };
}

entt::entity IconSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    std::string path = data.value("path", "");
    float scale = data.value("scale", 0.5f);
    auto &iconComp = dcx.world->AddComponent<IconComponent>(entity, path, scale);
    iconComp.billboardMode = data.value("billboardMode", true);

    return entity;
}

bool IconSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<IconComponent>(entity);
}