#include "ManifoldCollector.h"

void ManifoldCollector::AddContact(const ContactInfo &contact) {
    // try to find the previously existing manifold for contacts between these two bodies
    int foundIdx = -1;
    for (int i = 0; i < m_manifolds.size(); i++) {
        const Manifold &m = m_manifolds[i];
        const bool hasA = m.m_bodyA == contact.bodyA || m.m_bodyB == contact.bodyA;
        const bool hasB = m.m_bodyB == contact.bodyB || m.m_bodyA == contact.bodyB;
        if (hasA && hasB) {
            foundIdx = i;
            break;
        }
    }

    // add contact to manifold
    if (foundIdx >= 0)
        m_manifolds[foundIdx].AddContact(contact);
    else {
        Manifold manifold;
        manifold.m_bodyA = contact.bodyA;
        manifold.m_bodyB = contact.bodyB;

        manifold.AddContact(contact);
        m_manifolds.push_back(manifold);
    }
}

void ManifoldCollector::PreSolve(const float dt) {
    for (auto &manifold : m_manifolds)
        manifold.PreSolve(dt);
}

void ManifoldCollector::Solve() {
    for (auto &manifold : m_manifolds)
        manifold.Solve();
}

void ManifoldCollector::PostSolve() {
    for (auto &manifold : m_manifolds)
        manifold.PostSolve();
}

void ManifoldCollector::RemoveExpired() {
    // remove expired manifolds
    for (int i = static_cast<int>(m_manifolds.size()) - 1; i >= 0; i--) {
        Manifold &manifold = m_manifolds[i];
        manifold.RemoveExpiredContacts();

        if (0 == manifold.GetNumContacts())
            m_manifolds.erase(m_manifolds.begin() + i);
    }
}
