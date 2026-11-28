#pragma once

#include <entt/entt.hpp>

struct c_Hierarchy {
    entt::entity parent = entt::null;

    entt::entity firstChild = entt::null;
    entt::entity lastChild  = entt::null;

    entt::entity prevSibling = entt::null;
    entt::entity nextSibling = entt::null;

    c_Hierarchy() = default;

    void Clear() {
        parent = entt::null;
        prevSibling = entt::null;
        nextSibling = entt::null;
    }

    bool HasParent()   const { return parent != entt::null; }
    bool HasChildren() const { return firstChild != entt::null; }
};