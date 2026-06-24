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

void IconSerializer::Deserialize(ECSWorld *world, entt::entity entity, const json &data) {
    std::string path = data.value("path", "");
    float scale = data.value("scale", 0.5f);
    auto &iconComp = world->AddComponent<IconComponent>(entity, path, scale);
    iconComp.billboardMode = data.value("billboardMode", true);
}

bool IconSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<IconComponent>(entity);
}