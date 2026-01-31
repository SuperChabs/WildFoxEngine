module;

#include <algorithm>
#include <vector>
#include <entt.hpp>
#include "entity/fwd.hpp"

export module WFE.ECS.Components.Hierarchy;

/**
 * @brief Hierarchy component
 * Responsible for Parent-Child something
 */

export struct HierarchyComponent
{
    entt::entity parent = entt::null;
    std::vector<entt::entity> children;

    HierarchyComponent() = default;

    bool HasParent()  const { return parent != entt::null; }
    bool HasCildren() const { return !children.empty(); }

    void AddChild(entt::entity child) { children.push_back(child); }
    void RemoveChild(entt::entity child)
    {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end())
            children.erase(it);
    }
};