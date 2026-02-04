module;

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <string>
#include <cstdint>

export module WFE.ECS.ECSWorld;

import WFE.ECS.Components;
import WFE.Core.Logger;

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

    std::vector<entt::entity> GetChildren(entt::entity entity)
    {
        if (!IsValid(entity) || !HasComponent<HierarchyComponent>(entity))
            return {};
        
        return GetComponent<HierarchyComponent>(entity).children;
    }
    
    entt::entity GetParent(entt::entity entity)
    {
        if (!IsValid(entity) || !HasComponent<HierarchyComponent>(entity))
            return entt::null;
        
        return GetComponent<HierarchyComponent>(entity).parent;
    }
    
    glm::mat4 GetGlobalTransform(entt::entity entity)
    {
        if (!IsValid(entity) || !HasComponent<TransformComponent>(entity))
            return glm::mat4(1.0f);
        
        auto& transform = GetComponent<TransformComponent>(entity);
        glm::mat4 localTransform = transform.GetModelMatrix();
        
        if (HasComponent<HierarchyComponent>(entity))
        {
            auto& hierarchy = GetComponent<HierarchyComponent>(entity);
            if (hierarchy.HasParent())
            {
                glm::mat4 parentTransform = GetGlobalTransform(hierarchy.parent);
                return parentTransform * localTransform;
            }
        }
        
        return localTransform;
    }

    void SetParent(entt::entity child, entt::entity parent)
    {
        if (!IsValid(child) || !IsValid(parent))
            return;
        
        if (!HasComponent<HierarchyComponent>(child))
            AddComponent<HierarchyComponent>(child);
        
        if (!HasComponent<HierarchyComponent>(parent))
            AddComponent<HierarchyComponent>(parent);
        
        auto& childHierarchy = GetComponent<HierarchyComponent>(child);
        auto& parentHierarchy = GetComponent<HierarchyComponent>(parent);
        
        if (childHierarchy.HasParent())
        {
            auto& oldParentHierarchy = GetComponent<HierarchyComponent>(childHierarchy.parent);
            oldParentHierarchy.RemoveChild(child);
        }
        
        childHierarchy.parent = parent;
        parentHierarchy.AddChild(child);
        
        Logger::Log(LogLevel::DEBUG, "Set parent relationship");
    }
    
    void ClearParent(entt::entity child)
    {
        if (!IsValid(child) || !HasComponent<HierarchyComponent>(child))
            return;
        
        auto& childHierarchy = GetComponent<HierarchyComponent>(child);
        
        if (childHierarchy.HasParent())
        {
            auto& parentHierarchy = GetComponent<HierarchyComponent>(childHierarchy.parent);
            parentHierarchy.RemoveChild(child);
            childHierarchy.parent = entt::null;
        }
    }

    entt::entity CreateCamera(const std::string& name = "Camera", bool setAsMain = false, bool isGameCamera = false)
    {
        auto entity = CreateEntity(name);
        
        AddComponent<TransformComponent>(entity, glm::vec3(0, 0, 3), glm::vec3(0), glm::vec3(1));
        AddComponent<CameraComponent>(entity);
        AddComponent<CameraOrientationComponent>(entity);
        
        auto cameraType = isGameCamera ? CameraTypeComponent::Type::GAME : CameraTypeComponent::Type::EDITOR;
        AddComponent<CameraTypeComponent>(entity, cameraType);
        
        auto& camera = GetComponent<CameraComponent>(entity);
        camera.isMainCamera = setAsMain;
        
        Logger::Log(LogLevel::INFO, "Camera entity created: " + name + 
                    (isGameCamera ? " (GAME)" : " (EDITOR)"));
        return entity;
    }

    entt::entity FindEditorCamera()
    {
        entt::entity result = entt::null;
        
        Each<CameraComponent, CameraTypeComponent>([&](entt::entity entity, CameraComponent& cam, CameraTypeComponent& type)
        {
            if (type.type == CameraTypeComponent::Type::EDITOR && cam.isActive)
                result = entity;
        });
        
        return result;
    }

    entt::entity FindGameCamera()
    {
        entt::entity result = entt::null;
        
        Each<CameraComponent, CameraTypeComponent>([&](entt::entity entity, CameraComponent& cam, CameraTypeComponent& type)
        {
            if (type.type == CameraTypeComponent::Type::GAME && cam.isActive)
                result = entity;
        });
        
        return result;
    }
};