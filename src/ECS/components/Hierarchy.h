#pragma once

#include <unordered_set>
#include <entt/entt.hpp>

struct c_Hierarchy {
    entt::entity parent = entt::null;
    std::unordered_set<entt::entity> children;

    c_Hierarchy() = default;

    bool HasParent() const { return parent != entt::null; }
    bool HasChildren() const { return !children.empty(); }
    void AddChild(entt::entity child) { children.insert(child); }
    void RemoveChild(entt::entity child) { children.erase(child); }
};