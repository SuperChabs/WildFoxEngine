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
    auto entity = registry.create();

    registry.emplace<IDComponent>(entity, nextID++);
    registry.emplace<TagComponent>(entity, name);

    Logger::Log(LogLevel::DEBUG, "Entity created: " + name);
    return entity;
}

void ECSWorld::DestroyEntity(entt::entity entity) {
    if (registry.valid(entity))
        registry.destroy(entity);
}

bool ECSWorld::IsValid(entt::entity entity) const {
    return registry.valid(entity);
}

void ECSWorld::Clear() {
    registry.clear();
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
        return glm::mat4(1.0f);

    if (depth > 64) {
        Logger::Log(LogLevel::ERROR, "Hierarchy cycle detected!");
        return glm::mat4(1.0f);
    }

    auto &transform = GetComponent<TransformComponent>(entity);
    glm::mat4 localTransform = transform.GetModelMatrix();

    if (HasComponent<HierarchyComponent>(entity)) {
        auto &hierarchy = GetComponent<HierarchyComponent>(entity);
        if (hierarchy.HasParent())
            return GetGlobalTransform(hierarchy.parent, depth + 1) * localTransform;
    }

    return localTransform;
}

void ECSWorld::SetParent(entt::entity child, entt::entity parent) {
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

    auto &childHierarchy = GetComponent<HierarchyComponent>(child);

    if (childHierarchy.HasParent()) {
        auto &parentHierarchy = GetComponent<HierarchyComponent>(childHierarchy.parent);
        parentHierarchy.RemoveChild(child);
        childHierarchy.parent = entt::null;
    }
}

entt::entity ECSWorld::CreateCamera(const std::string &name, bool setAsMain, bool isGameCamera) {
    auto entity = CreateEntity(name);

    AddComponent<TransformComponent>(entity, glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(1));
    AddComponent<CameraComponent>(entity);
    AddComponent<CameraOrientationComponent>(entity);
    AddComponent<VisibilityComponent>(entity, true);

    auto cameraType = isGameCamera ? CameraTypeComponent::Type::GAME : CameraTypeComponent::Type::EDITOR;
    AddComponent<CameraTypeComponent>(entity, cameraType);

    auto &camera = GetComponent<CameraComponent>(entity);
    camera.isMainCamera = setAsMain;

    Logger::Log(LogLevel::INFO, "Camera entity created: " + name +
                                (isGameCamera ? " (GAME)" : " (EDITOR)"));
    return entity;
}

entt::entity ECSWorld::FindEditorCamera() {
    entt::entity result = entt::null;

    Each<CameraComponent, CameraTypeComponent>(
        [&](entt::entity entity, CameraComponent &cam, CameraTypeComponent &type) {
            if (type.type == CameraTypeComponent::Type::EDITOR && cam.isActive)
                result = entity;
        });

    return result;
}

entt::entity ECSWorld::FindGameCamera() {
    entt::entity result = entt::null;

    Each<CameraComponent, CameraTypeComponent>(
        [&](entt::entity entity, CameraComponent &cam, CameraTypeComponent &type) {
            if (type.type == CameraTypeComponent::Type::GAME && cam.isActive)
                result = entity;
        });

    return result;
}

// --- split_headers: auto-generated ---

entt::registry &ECSWorld::GetRegistry() {
    return registry;
}