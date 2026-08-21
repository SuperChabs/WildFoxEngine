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

    registry.emplace<IDComponent>(entity, nextID++);
    registry.emplace<TagComponent>(entity, name);

    Logger::Log(LogLevel::DEBUG, "Entity created: " + name);
    return entity;
}

void ECSWorld::DestroyEntity(entt::entity entity) {
    if (!IsValid(entity))
        return;

    if (HasComponent<HierarchyComponent>(entity)) {
        auto children = GetComponent<HierarchyComponent>(entity).children;

        for (auto &child : children)
            DestroyEntity(child);
    }

    if (HasComponent<HierarchyComponent>(entity)) {
        auto &hc = GetComponent<HierarchyComponent>(entity);
        if (hc.HasParent() && IsValid(hc.parent) && HasComponent<HierarchyComponent>(hc.parent))
            GetComponent<HierarchyComponent>(hc.parent).RemoveChild(entity);
    }

    if (registry.valid(entity))
        registry.destroy(entity);
}

bool ECSWorld::IsValid(const entt::entity entity) const {
    return registry.valid(entity);
}

void ECSWorld::Clear() {
    std::vector<entt::entity> entities;

    for (auto entity : registry.view<IDComponent>())
    {
        if (HasComponent<CameraComponent>(entity))
            continue;

        entities.push_back(entity);
    }

    for (auto entity : entities)
        DestroyEntity(entity);

    nextID = 1;
}

size_t ECSWorld::GetEntityCount() const {
    return registry.storage<entt::entity>()->size();
}

std::unordered_set<entt::entity> ECSWorld::GetChildren(entt::entity entity) {
    if (!IsValid(entity) || !HasComponent<HierarchyComponent>(entity))
        return {};

    return GetComponent<HierarchyComponent>(entity).children;
}

entt::entity ECSWorld::GetParent(entt::entity entity) {
    if (!IsValid(entity) || !HasComponent<HierarchyComponent>(entity))
        return entt::null;

    return GetComponent<HierarchyComponent>(entity).parent;
}

glm::mat4 ECSWorld::GetGlobalTransform(entt::entity entity, int depth) {
    if (!IsValid(entity) || !HasComponent<TransformComponent>(entity))
        return {1.0f};

    if (depth > 64) {
        Logger::Log(LogLevel::ERROR, "Hierarchy cycle detected!");
        return {1.0f};
    }

    const auto &transform = GetComponent<TransformComponent>(entity);
    const glm::mat4 localTransform = transform.GetModelMatrix();

    if (HasComponent<HierarchyComponent>(entity)) {
        auto &hierarchy = GetComponent<HierarchyComponent>(entity);
        if (hierarchy.HasParent())
            return GetGlobalTransform(hierarchy.parent, depth + 1) * localTransform;
    }

    return localTransform;
}

void ECSWorld::SetParent(const entt::entity child, const entt::entity parent) {
    if (!IsValid(child) || !IsValid(parent))
        return;

    if (!HasComponent<HierarchyComponent>(child))
        AddComponent<HierarchyComponent>(child);

    if (!HasComponent<HierarchyComponent>(parent))
        AddComponent<HierarchyComponent>(parent);

    auto &childHierarchy = GetComponent<HierarchyComponent>(child);
    auto &parentHierarchy = GetComponent<HierarchyComponent>(parent);

    if (childHierarchy.HasParent()) {
        auto &oldParentHierarchy = GetComponent<HierarchyComponent>(childHierarchy.parent);
        oldParentHierarchy.RemoveChild(child);
    }

    childHierarchy.parent = parent;
    parentHierarchy.AddChild(child);

    Logger::Log(LogLevel::DEBUG, "Set parent relationship");
}

void ECSWorld::ClearParent(entt::entity child) {
    if (!IsValid(child) || !HasComponent<HierarchyComponent>(child))
        return;

    if (auto &childHierarchy = GetComponent<HierarchyComponent>(child); childHierarchy.HasParent()) {
        auto &parentHierarchy = GetComponent<HierarchyComponent>(childHierarchy.parent);
        parentHierarchy.RemoveChild(child);
        childHierarchy.parent = entt::null;
    }
}

entt::entity ECSWorld::CreateCamera(const std::string &name, bool setAsMain) {
    const auto entity = CreateEntity(name);

    AddComponent<TransformComponent>(entity, glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(1));
    AddComponent<CameraComponent>(entity);
    AddComponent<CameraOrientationComponent>(entity);
    AddComponent<VisibilityComponent>(entity, true);
    AddComponent<IconComponent>(entity, "assets/textures/icons/camera.png", 0.3f);

    auto &camera = GetComponent<CameraComponent>(entity);
    camera.isMainCamera = setAsMain;

    Logger::Log(LogLevel::INFO, "Camera entity created: " + name);
    return entity;
}

entt::registry &ECSWorld::GetRegistry() {
    return registry;
}