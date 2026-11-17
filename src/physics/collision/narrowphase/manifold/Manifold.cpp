#include "Manifold.h"

#include "physics/dynamics/BodyTransform.h"

void Manifold::AddContact(const ContactInfo &contactOld) {
    // make sure the contact's bodyA an bodyB are of the correct order
    ContactInfo contact = contactOld;
    if (contactOld.bodyA != m_bodyA || contactOld.bodyB != m_bodyB) {
        contact.ptOnA_LocalSpace = contactOld.ptOnB_LocalSpace;
        contact.ptOnB_LocalSpace = contactOld.ptOnA_LocalSpace;
        contact.ptOnA_WorldSpace = contactOld.ptOnB_WorldSpace;
        contact.ptOnB_WorldSpace = contactOld.ptOnA_WorldSpace;

        // contact.normal = -contactOld.normal;

        contact.bodyA = m_bodyA;
        contact.bodyB = m_bodyB;
    }

    // if this contact is too close to another contact, then keep the old contact
    for (int i = 0; i < m_numContacts; i++) {
        const BodyInfo *bodyA = m_contacts[i].bodyA;
        const BodyInfo *bodyB = m_contacts[i].bodyB;

        const glm::vec3 oldA = BodyTransform::BodySpaceToWorldSpace(*bodyA, m_contacts[i].ptOnA_LocalSpace);
        const glm::vec3 oldB = BodyTransform::BodySpaceToWorldSpace(*bodyB, m_contacts[i].ptOnB_LocalSpace);

        const glm::vec3 newA = BodyTransform::BodySpaceToWorldSpace(*contact.bodyA, contact.ptOnA_LocalSpace);
        const glm::vec3 newB = BodyTransform::BodySpaceToWorldSpace(*contact.bodyB, contact.ptOnB_LocalSpace);

        const glm::vec3 aa = newA - oldA;
        const glm::vec3 bb = newB - oldB;

        constexpr float distanceThreshold = 0.02f;
        if (glm::dot(aa, aa) < distanceThreshold * distanceThreshold)
            return;
        if (glm::dot(bb, bb) < distanceThreshold * distanceThreshold)
            return;
    }

    // if we're all full of contacts, then keep the contacts that are furthest away from each other
    int newSlot = m_numContacts;
    if (newSlot >= MAX_CONTACT_POINTS) {
        glm::vec3 avg{0.0f};
        avg += m_contacts[0].ptOnA_LocalSpace;
        avg += m_contacts[1].ptOnA_LocalSpace;
        avg += m_contacts[2].ptOnA_LocalSpace;
        avg += m_contacts[3].ptOnA_LocalSpace;
        avg += contact.ptOnA_LocalSpace;
        avg *= 0.2f;

        float minDist = glm::dot(avg - contact.ptOnA_LocalSpace, avg - contact.ptOnA_LocalSpace);
        int newIdx = -1;
        for (int i = 0; i < MAX_CONTACT_POINTS; i++) {
            float dist2 = glm::dot(avg - m_contacts[i].ptOnA_LocalSpace, avg - m_contacts[i].ptOnA_LocalSpace);

            if (dist2 < minDist) {
                minDist = dist2;
                newIdx = i;
            }
        }

        if (-1 != newIdx)
            newSlot = newIdx;
        else
            return;
    }

    m_contacts[newSlot] = contact;

    m_constraints[newSlot].m_bodyA = contact.bodyA;
    m_constraints[newSlot].m_bodyB = contact.bodyB;
    m_constraints[newSlot].m_anchorA = contact.ptOnA_LocalSpace;
    m_constraints[newSlot].m_anchorB = contact.ptOnB_LocalSpace;

    // get the normal in body's space
    const glm::vec3 normal = glm::inverse(m_bodyA->transform->rotation) * (contact.normal * -1.0f);
    m_constraints[newSlot].m_normal = normal;
    m_constraints[newSlot].m_normal = glm::normalize(m_constraints[newSlot].m_normal);

    m_constraints[newSlot].m_cachedLambda.Zero();

    if (newSlot == m_numContacts)
        m_numContacts++;
}

void Manifold::RemoveExpiredContacts() {
    // remove any contact that have drifted too far
    for (int i = 0; i < m_numContacts; i++) {
        ContactInfo &contact = m_contacts[i];

        const BodyInfo *bodyA = contact.bodyA;
        const BodyInfo *bodyB = contact.bodyB;

        // get the tangential distance of the point on A and the point on B
        const glm::vec3 a = BodyTransform::BodySpaceToWorldSpace(*bodyA, contact.ptOnA_LocalSpace);
        const glm::vec3 b = BodyTransform::BodySpaceToWorldSpace(*bodyB, contact.ptOnB_LocalSpace);

        glm::vec3 normal = m_constraints[i].m_normal;
        normal = bodyA->transform->rotation * normal;

        // calculate the tangential separation and penetration depth
        const glm::vec3 ab = b - a;
        const float penetrationDepth = glm::dot(normal, ab);
        glm::vec3 abNormal = normal * penetrationDepth;
        glm::vec3 abTangent = ab - abNormal;

        // if the tangential displacement is less than a specific threshold, it's okay to keep it
        constexpr float distanceThreshold = 0.02f;
        if (glm::dot(abTangent, abTangent) < distanceThreshold * distanceThreshold && penetrationDepth <= 0.0f)
            continue;

        // this contact has moved beyond its threshold and should be removed
        for (int j = i; j < MAX_CONTACT_POINTS - 1; j++) {
            m_constraints[j] = m_constraints[j + 1];
            m_contacts[j] = m_contacts[j + 1];
            if (j >= m_numContacts)
                m_constraints[j].m_cachedLambda.Zero();
        }
        m_numContacts--;
        i--;
    }
}

void Manifold::PreSolve(const float dt) {
    for (int i = 0; i < m_numContacts; i++)
        m_constraints[i].PreSolve(dt);
}

void Manifold::Solve() {
    for (int i = 0; i < m_numContacts; i++)
        m_constraints[i].Solve();
}

void Manifold::PostSolve() {
    for (int i = 0; i < m_numContacts; i++)
        m_constraints[i].PostSolve();
}
