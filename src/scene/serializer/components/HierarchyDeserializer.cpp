#include "HierarchyDeserializer.h"

HierarchyDeserializer::HierarchyDeserializer(ECSWorld *w, const std::unordered_map<uint64_t, entt::entity> &entityMap)
    : world(w), idMap(entityMap) {
}

void HierarchyDeserializer::SetupHierarchy(const json &sceneData) {
    for (const auto &entityData: sceneData["scene"]["entities"]) {
        if (!entityData.contains("_parentId"))
            continue;

        uint64_t childUUID = entityData["_id"];
        uint64_t parentUUID = entityData["_parentId"];

        auto childIt = idMap.find(childUUID);
        auto parentIt = idMap.find(parentUUID);

        if (childIt != idMap.end() && parentIt != idMap.end()) {
            world->SetParent(childIt->second, parentIt->second);
        }
    }
}