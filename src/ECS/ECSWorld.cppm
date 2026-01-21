module;

#include <entt.hpp>
#include <string>
#include <cstdint>

export module WildFoxEngine.ECS.ECSWorld;

import WildFoxEngine.ECS.Components;
import WildFoxEngine.Core.Logger;

export class ECSWorld 
{
private:
    entt::registry registry;
    uint64_t nextID = 1;
    
public:
    ECSWorld() 
    {
        Logger::Log(LogLevel::INFO, "ECS World initialized");
    }
    
    ~ECSWorld()
    {
        Clear();
    }
    
    entt::entity CreateEntity(const std::string& name = "Entity") 
    {
        auto entity = registry.create();
        
        registry.emplace<IDComponent>(entity, nextID++);
        registry.emplace<TagComponent>(entity, name);
        
        Logger::Log(LogLevel::DEBUG, "Entity created: " + name);
        return entity;
    }
    
    void DestroyEntity(entt::entity entity) 
    {
        if (registry.valid(entity)) 
            registry.destroy(entity);
    }
    
    bool IsValid(entt::entity entity) const 
    {
        return registry.valid(entity);
    }
    
    template<typename T, typename... Args>
    T& AddComponent(entt::entity entity, Args&&... args) 
    {
        return registry.emplace<T>(entity, std::forward<Args>(args)...);
    }
    
    template<typename T>
    T& GetComponent(entt::entity entity) 
    {
        return registry.get<T>(entity);
    }
    
    template<typename T>
    bool HasComponent(entt::entity entity) const 
    {
        return registry.all_of<T>(entity);
    }
    
    template<typename T>
    void RemoveComponent(entt::entity entity) 
    {
        if (registry.all_of<T>(entity))
            registry.remove<T>(entity);
    }
    
    template<typename... Components>
    auto View() 
    {
        return registry.view<Components...>();
    }
    
    template<typename... Components, typename Func>
    void Each(Func&& func) 
    {
        registry.view<Components...>().each(std::forward<Func>(func));
    }
    
    void Clear() 
    {
        registry.clear();
        nextID = 1;
    }
    
    size_t GetEntityCount() const 
    {
        return registry.storage<entt::entity>()->size();
    }
    
    entt::registry& GetRegistry() { return registry; }
};