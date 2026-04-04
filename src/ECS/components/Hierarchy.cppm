module;

#include <string>
#include <memory>
#include <vector>
#include <entt/entt.hpp>

export module WFE.ECS.Components.Hierarchy;

export struct HierarchyComponent
{
    entt::entity parent = entt::null;
    std::vector<entt::entity> children;

    HierarchyComponent() = default;

    bool HasParent()   const;
    bool HasChildren() const;
    void AddChild(entt::entity child);
    void RemoveChild(entt::entity child);
};

bool HierarchyComponent::HasParent()   const { return parent != entt::null; }
bool HierarchyComponent::HasChildren() const { return !children.empty(); }

void HierarchyComponent::AddChild(entt::entity child)
{
    children.push_back(child);
}

void HierarchyComponent::RemoveChild(entt::entity child)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
        children.erase(it);
}