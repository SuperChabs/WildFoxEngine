#pragma once
#include "Manifold.h"

class ManifoldCollector {
public:
    std::vector<Manifold> m_manifolds;

    ManifoldCollector() = default;

    void AddContact(const ContactInfo& contact);

    void PreSolve(float dt);
    void Solve();
    void PostSolve();

    void RemoveExpired();
    void Clear() { m_manifolds.clear(); }
};
