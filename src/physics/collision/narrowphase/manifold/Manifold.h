#pragma once

#include "physics/core/PhysicsContext.h"
#include "../support/constraint/ConstraintPenetration.h"

class Manifold {
    static constexpr int MAX_CONTACT_POINTS = 4;
    ContactInfo m_contacts[MAX_CONTACT_POINTS] = {};

    int m_numContacts;

    BodyInfo *m_bodyA;
    BodyInfo *m_bodyB;

    ConstraintPenetration m_constraints[MAX_CONTACT_POINTS];

    friend class ManifoldCollector;

public:
    Manifold() : m_numContacts(0), m_bodyA(nullptr), m_bodyB(nullptr) {}

    void AddContact(const ContactInfo &contactOld);
    void RemoveExpiredContacts();

    void PreSolve(float dt);
    void Solve();
    void PostSolve();

    [[nodiscard]] ContactInfo GetContact(const int idx) const { return m_contacts[idx]; }
    [[nodiscard]] int GetNumContacts() const { return m_numContacts; }
};
