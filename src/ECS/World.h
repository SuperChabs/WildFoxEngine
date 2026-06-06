#pragma once

#include <string>
#include <type_traits>
#include <unordered_set>
#include <entt/entt.hpp>
#include <glm/glm.hpp>


class ECSWorld {
private:
    entt::registry registry;
    uint64_t nextID = 1;

public:
    ECSWorld();

    ~ECSWorld();

    entt::entity CreateEntity(const std::string &name = "Entity");

    void DestroyEntity(entt::entity entity);

    template<typename T, typename... Args>
    decltype(auto) AddComponent(entt::entity entity, Args &&... args) {
        return registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    decltype(auto) AddComponent(entt::entity entity) {
        return registry.emplace_or_replace<T>(entity);
    }

    template<typename T>
    T &GetComponent(entt::entity entity) {
        return registry.get<T>(entity);
    }

    template<typename T>
    bool HasComponent(entt::entity entity) const {
        return registry.all_of<T>(entity);
    }

    template<typename T>
    void RemoveComponent(entt::entity entity) {
        if (registry.all_of<T>(entity))
            registry.remove<T>(entity);
    }

    template<typename... Components>
    auto View() {
        return registry.view<Components...>();
    }

    template<typename... Components, typename Func>
    void Each(Func &&func) {
        registry.view<Components...>().each(std::forward<Func>(func));
    }

    void Clear();

    void SetParent(entt::entity child, entt::entity parent);

    void ClearParent(entt::entity child);

    entt::entity CreateCamera(const std::string &name = "Camera", bool setAsMain = false, bool isGameCamera = false);

    entt::entity FindEditorCamera();

    entt::entity FindGameCamera();

    glm::mat4 GetGlobalTransform(entt::entity entity, int depth = 0);

    entt::entity GetParent(entt::entity entity);

    std::unordered_set<entt::entity> GetChildren(entt::entity entity);

    entt::registry &GetRegistry() { return registry; }

    size_t GetEntityCount() const;

    bool IsValid(entt::entity entity) const;
};