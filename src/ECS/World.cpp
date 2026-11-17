#include "World.h"

#include "core/logging/Logger.h"
#include "ECS/components/Components.h"

ECSWorld::ECSWorld() {
    Logger::Log(LogLevel::INFO, "ECS World initialized");
}

ECSWorld::~ECSWorld() {
    Clear();
}

entt::entity ECSWorld::CreateEntity(const std::string &name) {
    const auto entity = registry.create();

    registry.emplace<c_ID>(entity, nextID++);
    registry.emplace<c_Tag>(entity, name);

    Logger::Log(LogLevel::DEBUG, "Entity created: " + name);
    return entity;
}

void ECSWorld::DestroyEntity(const entt::entity entity) {
    if (!IsValid(entity))
        return;

    if (HasComponent<c_Hierarchy>(entity))
        for (const auto children = GetComponent<c_Hierarchy>(entity).children; auto &child: children)
            DestroyEntity(child);

    if (HasComponent<c_Hierarchy>(entity))
        if (const auto &hc = GetComponent<c_Hierarchy>(entity); hc.HasParent()
            && IsValid(hc.parent) && HasComponent<c_Hierarchy>(hc.parent))
            GetComponent<c_Hierarchy>(hc.parent).RemoveChild(entity);

    if (registry.valid(entity))
        registry.destroy(entity);
}

bool ECSWorld::IsValid(const entt::entity entity) const {
    return registry.valid(entity);
}

void ECSWorld::Clear() {
    std::vector<entt::entity> entities;

    for (auto entity: registry.view<c_ID>()) {
        if (HasComponent<c_Persistent>(entity))
            continue;

        entities.push_back(entity);
    }

    for (const auto entity: entities)
        DestroyEntity(entity);

    nextID = 1;
}

size_t ECSWorld::GetEntityCount() const {
    return registry.storage<entt::entity>()->size();
}

std::unordered_set<entt::entity> ECSWorld::GetChildren(entt::entity entity) {
    if (!IsValid(entity) || !HasComponent<c_Hierarchy>(entity))
        return {};

    return GetComponent<c_Hierarchy>(entity).children;
}

entt::entity ECSWorld::GetParent(entt::entity entity) {
    if (!IsValid(entity) || !HasComponent<c_Hierarchy>(entity))
        return entt::null;

    return GetComponent<c_Hierarchy>(entity).parent;
}

glm::mat4 ECSWorld::GetGlobalTransform(entt::entity entity, int depth) {
    if (!IsValid(entity) || !HasComponent<c_Transform>(entity))
        return {1.0f};

    if (depth > 64) {
        Logger::Log(LogLevel::ERROR, "Hierarchy cycle detected!");
        return {1.0f};
    }

    const auto &transform = GetComponent<c_Transform>(entity);
    const glm::mat4 localTransform = transform.GetModelMatrix();

    if (HasComponent<c_Hierarchy>(entity)) {
        auto &hierarchy = GetComponent<c_Hierarchy>(entity);
        if (hierarchy.HasParent())
            return GetGlobalTransform(hierarchy.parent, depth + 1) * localTransform;
    }

    return localTransform;
}

void ECSWorld::SetParent(const entt::entity child, const entt::entity parent) {
    if (!IsValid(child) || !IsValid(parent))
        return;

    if (!HasComponent<c_Hierarchy>(child))
        AddComponent<c_Hierarchy>(child);

    if (!HasComponent<c_Hierarchy>(parent))
        AddComponent<c_Hierarchy>(parent);

    auto &childHierarchy = GetComponent<c_Hierarchy>(child);
    auto &parentHierarchy = GetComponent<c_Hierarchy>(parent);

    if (childHierarchy.HasParent()) {
        auto &oldParentHierarchy = GetComponent<c_Hierarchy>(childHierarchy.parent);
        oldParentHierarchy.RemoveChild(child);
    }

    childHierarchy.parent = parent;
    parentHierarchy.AddChild(child);

    Logger::Log(LogLevel::DEBUG, "Set parent relationship");
}

void ECSWorld::ClearParent(entt::entity child) {
    if (!IsValid(child) || !HasComponent<c_Hierarchy>(child))
        return;

    if (auto &childHierarchy = GetComponent<c_Hierarchy>(child); childHierarchy.HasParent()) {
        auto &parentHierarchy = GetComponent<c_Hierarchy>(childHierarchy.parent);
        parentHierarchy.RemoveChild(child);
        childHierarchy.parent = entt::null;
    }
}

entt::entity ECSWorld::CreateCamera(const std::string &name, bool setAsMain) {
    const auto entity = CreateEntity(name);

    AddComponent<c_Transform>(entity, glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(1));
    AddComponent<c_Camera>(entity);
    AddComponent<c_CameraOrientation>(entity);
    AddComponent<c_Visibility>(entity, true);
    AddComponent<c_Icon>(entity, "assets/textures/icons/camera.png", 0.3f);

    auto &camera = GetComponent<c_Camera>(entity);
    camera.isMainCamera = setAsMain;

    Logger::Log(LogLevel::INFO, "Camera entity created: " + name);
    return entity;
}

entt::registry &ECSWorld::GetRegistry() {
    return registry;
}