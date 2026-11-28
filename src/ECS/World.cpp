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

    while (HasComponent<c_Hierarchy>(entity)) {
        const auto child = GetComponent<c_Hierarchy>(entity).firstChild;

        if (child == entt::null) break;

        if (!IsValid(child)) {
            Logger::Log(LogLevel::ERROR, "Invalid child entity detected!");
            break;
        }

        DestroyEntity(child);
    }

    if (HasComponent<c_Hierarchy>(entity))
        Detach(entity);

    if (registry.valid(entity))
        registry.destroy(entity);
}

void ECSWorld::Detach(const entt::entity child) {
    auto &childHierarchy = GetComponent<c_Hierarchy>(child);

    if (childHierarchy.parent != entt::null) {
        auto &parentHierarchy = GetComponent<c_Hierarchy>(childHierarchy.parent);

        const auto prevSibling = childHierarchy.prevSibling;
        const auto nextSibling = childHierarchy.nextSibling;

        if (prevSibling != entt::null) {
            auto &prevHierarchy = GetComponent<c_Hierarchy>(prevSibling);
            prevHierarchy.nextSibling = nextSibling;
        }
        else
            parentHierarchy.firstChild = nextSibling;

        if (nextSibling != entt::null) {
            auto &nextHierarchy = GetComponent<c_Hierarchy>(nextSibling);
            nextHierarchy.prevSibling = prevSibling;
        }
        else
            parentHierarchy.lastChild = prevSibling;
    }

    childHierarchy.Clear();
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

void ECSWorld::SetParent(const entt::entity child, const entt::entity newParent) {
    if (!IsValid(newParent) || !IsValid(child)) return;

    entt::entity temp = newParent;
    while (temp != entt::null) {
        if (!HasComponent<c_Hierarchy>(temp)) break;

        if (child == temp) return;

        temp = GetComponent<c_Hierarchy>(temp).parent;
    }

    if (!HasComponent<c_Hierarchy>(newParent))
        AddComponent<c_Hierarchy>(newParent);

    if (!HasComponent<c_Hierarchy>(child))
        AddComponent<c_Hierarchy>(child);

    Detach(child);

    auto &parentHierarchy = GetComponent<c_Hierarchy>(newParent);
    auto &childHierarchy  = GetComponent<c_Hierarchy>(child);
    if (parentHierarchy.lastChild != entt::null) {
        const auto oldLast  = parentHierarchy.lastChild;
        auto &prevHierarchy = GetComponent<c_Hierarchy>(parentHierarchy.lastChild);

        prevHierarchy.nextSibling  = child;
        childHierarchy.prevSibling = oldLast;
        parentHierarchy.lastChild  = child;
    }
    else {

        parentHierarchy.firstChild = child;
        parentHierarchy.lastChild = child;
    }

    childHierarchy.parent = newParent;
}

void ECSWorld::ClearParent(const entt::entity child) {
    if (!IsValid(child) || !HasComponent<c_Hierarchy>(child))
        return;

    auto &childHierarchy = GetComponent<c_Hierarchy>(child);
    if (childHierarchy.HasParent()) {
        Detach(child);
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