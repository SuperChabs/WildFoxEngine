#include "NarrowPhase.h"

#include "algorithms/GJK.h"
#include "math/Math.h"
#include "physics/dynamics/BodyTransform.h"
#include "physics/dynamics/Integrator.h"

bool NarrowPhase::Intersect(BodyInfo &bodyA, BodyInfo &bodyB, ContactInfo &contact, float dt) {
    contact.bodyA = &bodyA;
    contact.bodyB = &bodyB;

    const auto &colA = bodyA.collider;
    const auto &colB = bodyB.collider;
    const auto &t_a = bodyA.transform;
    const auto &t_b = bodyB.transform;

    const auto isUnitSphere = [](const c_Collider &collider, const c_Transform &transform) {
        if (!std::holds_alternative<Sphere>(collider.shape))
            return false;

        const auto &sphere = std::get<Sphere>(collider.shape);
        return sphere.m_center == glm::vec3(0.0f)
            && transform.scale == glm::vec3(1.0f);
    };
    const bool bothSpheres = isUnitSphere(*colA, *t_a) && isUnitSphere(*colB, *t_b);

    if (!bothSpheres)
        return ConservativeAdvance(bodyA, bodyB, contact, dt); // GJK

    const auto &sphereA = std::get<Sphere>(colA->shape);
    const auto &sphereB = std::get<Sphere>(colB->shape);

    const auto &r_a = bodyA.rigidBody;
    const auto &r_b = bodyB.rigidBody;

    if (!SphereSphereDynamic(&sphereA, &sphereB, t_a->position, t_b->position,
                             r_a->m_linearVelocity, r_b->m_linearVelocity, dt,
                             contact.ptOnA_WorldSpace, contact.ptOnB_WorldSpace,
                             contact.timeOfImpact))
        return false;

    // Step bodies forward to get local space collision points
    Integrator::IntegrateBody(bodyA, contact.timeOfImpact);
    Integrator::IntegrateBody(bodyB, contact.timeOfImpact);

    // Convert world space contacts to local space
    contact.ptOnA_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyA, contact.ptOnA_WorldSpace);
    contact.ptOnB_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyB, contact.ptOnB_WorldSpace);

    contact.normal = glm::normalize(t_a->position - t_b->position);

    // Unwind time step
    Integrator::IntegrateBody(bodyA, -contact.timeOfImpact);
    Integrator::IntegrateBody(bodyB, -contact.timeOfImpact);

    // Calculate the separation distance
    const glm::vec3 ab = t_b->position - t_a->position;
    contact.separationDistance = glm::length(ab) - (sphereA.m_radius + sphereB.m_radius);

    return true;
}

bool NarrowPhase::Intersect(BodyInfo &bodyA, BodyInfo &bodyB, ContactInfo &contact) {
    contact.bodyA = &bodyA;
    contact.bodyB = &bodyB;
    contact.timeOfImpact = 0.0f;

    const auto &colA = bodyA.collider;
    const auto &colB = bodyB.collider;
    const auto &t_a = bodyA.transform;
    const auto &t_b = bodyB.transform;

    const auto isUnitSphere = [](const c_Collider &collider, const c_Transform &transform) {
        if (!std::holds_alternative<Sphere>(collider.shape))
            return false;

        const Sphere &sphere = std::get<Sphere>(collider.shape);
        return sphere.m_center == glm::vec3(0.0f)
            && transform.scale == glm::vec3(1.0f);
    };
    if (isUnitSphere(*colA, *t_a) && isUnitSphere(*colB, *t_b)) {
        const auto &sphereA = std::get<Sphere>(colA->shape);
        const auto &sphereB = std::get<Sphere>(colB->shape);

        const glm::vec3 posA = t_a->position;
        const glm::vec3 posB = t_b->position;

        if (SphereSphereStatic(&sphereA, &sphereB, posA, posB, contact.ptOnA_WorldSpace, contact.ptOnB_WorldSpace)) {
            contact.normal = posA - posB;
            contact.normal = glm::normalize(contact.normal);

            contact.ptOnA_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyA, contact.ptOnA_WorldSpace);
            contact.ptOnB_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyB, contact.ptOnB_WorldSpace);

            glm::vec3 ab = t_b->position - t_a->position;
            float r = math::GetMagnitude(ab) - (sphereA.m_radius + sphereB.m_radius);
            contact.separationDistance = r;
            return true;
        }
    }
    else {
        glm::vec3 ptOnA;
        glm::vec3 ptOnB;
        constexpr float bias = 0.001f;
        if (GJK::DoesIntersect(bodyA, bodyB, bias, ptOnA, ptOnB)) {
            // there was an intersection, so get the contact data
            glm::vec3 normal = ptOnB - ptOnA;
            normal = glm::normalize(normal);

            ptOnA -= normal * bias;
            ptOnB += normal * bias;

            contact.normal = normal;

            contact.ptOnA_WorldSpace = ptOnA;
            contact.ptOnB_WorldSpace = ptOnB;

            contact.ptOnA_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyA, contact.ptOnA_WorldSpace);
            contact.ptOnB_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyB, contact.ptOnB_WorldSpace);

            const float r = math::GetMagnitude(ptOnA - ptOnB);
            contact.separationDistance = -r;
            return true;
        }

        // there was no collision, but we still want the contact data, so get it
        GJK::CLosestPoints(bodyA, bodyB, ptOnA, ptOnB);
        contact.ptOnA_WorldSpace = ptOnA;
        contact.ptOnB_WorldSpace = ptOnB;

        contact.ptOnA_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyA, contact.ptOnA_WorldSpace);
        contact.ptOnB_LocalSpace = BodyTransform::WorldSpaceToBodySpace(bodyB, contact.ptOnB_WorldSpace);

        const float r = math::GetMagnitude(ptOnA - ptOnB);
        contact.separationDistance = r;
    }

    return false;
}

bool NarrowPhase::ConservativeAdvance(BodyInfo &bodyA, BodyInfo &bodyB, ContactInfo &contact, float dt) {
    contact.bodyA = &bodyA;
    contact.bodyB = &bodyB;

    float toi = 0.0f;

    int numIters = 0;

    const auto &c_a = bodyA.collider;
    const auto &c_b = bodyB.collider;
    const auto &r_a = bodyA.rigidBody;
    const auto &r_b = bodyB.rigidBody;

    // advance the positions of the bodies until they touch or there's not time left
    while (dt > 0.0f) {
        // check for intersection
        if (Intersect(bodyA, bodyB, contact)) {
            contact.timeOfImpact = toi;
            Integrator::IntegrateBody(bodyA, -toi);
            Integrator::IntegrateBody(bodyB, -toi);
            return true;
        }

        ++numIters;
        if (numIters > 10) break;

        // get the vector from the closest point on A to the closest point on B
        glm::vec3 ab = contact.ptOnB_WorldSpace - contact.ptOnA_WorldSpace;
        if (ab == glm::vec3(0.0)) break;
        ab = glm::normalize(ab);

        // project the relative velocity onto the ray of shortest distance
        glm::vec3 relativeVelocity = r_a->m_linearVelocity - r_b->m_linearVelocity;
        float orthoSpeed = glm::dot(ab, relativeVelocity);

        // add to the orthoSpeed the maximum angular speeds of the relative shapes
        float angularSpeedA = c_a->FastestLinearSpeed(r_a->m_angularVelocity, ab);
        float angularSpeedB = c_b->FastestLinearSpeed(r_b->m_angularVelocity, ab * -1.0f);
        orthoSpeed += angularSpeedA + angularSpeedB;
        if (orthoSpeed <= 0.0f) break;

        float timeToGo = contact.separationDistance / orthoSpeed;
        if (timeToGo > dt) break;

        dt -= timeToGo;
        toi += timeToGo;
        Integrator::IntegrateBody(bodyA, timeToGo);
        Integrator::IntegrateBody(bodyB, timeToGo);
    }

    // unwind the clock
    Integrator::IntegrateBody(bodyA, -toi);
    Integrator::IntegrateBody(bodyB, -toi);
    return false;
}

bool NarrowPhase::RaySphere(const glm::vec3 &rayStart, const glm::vec3 &rayDir, const glm::vec3 &sphereCenter,
        const float &sphereRadius, float &t1, float &t2) {
    const glm::vec3 m = sphereCenter - rayStart;
    const float a = glm::dot(rayDir, rayDir);
    const float b = glm::dot(rayDir, m);
    const float c = glm::dot(m, m) - sphereRadius * sphereRadius;

    const float delta = b * b - a * c;
    const float invA = 1.0f / a;

    if (delta < 0.0f) return false;

    const float deltaRoot = std::sqrt(delta);
    t1 = invA * (b - deltaRoot);
    t2 = invA * (b + deltaRoot);

    return true;
}

bool NarrowPhase::SphereSphereDynamic(const Sphere *shapeA, const Sphere *shapeB, glm::vec3 posA, glm::vec3 posB,
        const glm::vec3 &velA, const glm::vec3 &velB, float dt, glm::vec3 &ptOnA, glm::vec3 &ptOnB, float &toi) {
    const glm::vec3 relativeVelocity = velA - velB;

    const glm::vec3 startPtA = posA;
    const glm::vec3 endPtA = posA + relativeVelocity * dt;
    const glm::vec3 rayDir = endPtA - startPtA;

    float t0 = 0, t1 = 0;
    if (glm::dot(rayDir, rayDir) < 0.001f * 0.001f) {
        // ray is too short, js chek if already intersecting
        const glm::vec3 ab = posB - posA;
        float radius = shapeA->m_radius + shapeB->m_radius + 0.001f;
        if (glm::dot(ab, ab) > radius * radius) return false;
    }
    else if (!RaySphere(startPtA, rayDir, posB, shapeA->m_radius + shapeB->m_radius, t0, t1))
        return false;

    // change from [0,1] range to [0,dt] range
    t0 *= dt;
    t1 *= dt;

    // if the collision is only in the past, then there's no future collision this frame
    if (t1 < 0.0f) return false;

    // get the earliest positive this time of impact
    toi = (t0 < 0.0f) ? 0.0f : t0;

    // if the earliest collision is too far in the future, then there's no collision this frame
    if (toi > dt) return false;

    // get the points on the respective points of collision and return true
    glm::vec3 newPosA = posA + velA * toi;
    glm::vec3 newPosB = posB + velB * toi;
    glm::vec3 ab = newPosB - newPosA;

    const float distanceSquared = glm::dot(ab, ab);

    glm::vec3 normal;

    if (distanceSquared > 1e-8f)
        normal = ab / glm::sqrt(distanceSquared);
    else {
        const glm::vec3 relativeVelocity = velB - velA;
        const float velocitySquared = glm::dot(relativeVelocity, relativeVelocity);

        if (velocitySquared > 1e-8f)
            normal = relativeVelocity / glm::sqrt(velocitySquared);
        else
            normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    ptOnA = newPosA + normal * shapeA->m_radius;
    ptOnB = newPosB - normal * shapeB->m_radius;
    return true;
}

bool NarrowPhase::SphereSphereStatic(const Sphere *shapeA, const Sphere *shapeB, const glm::vec3 posA, const glm::vec3 posB,
        glm::vec3 &ptOnA, glm::vec3 &ptOnB) {
    const glm::vec3 ab = posB - posA;
    const glm::vec3 norm = glm::dot(ab, ab) > 1e-8f ? glm::normalize(ab) : glm::vec3(0.0f, 0.1f, 0.0f);

    ptOnA = posA + norm * shapeA->m_radius;
    ptOnB = posB - norm * shapeB->m_radius;

    const float radiusAB = shapeA->m_radius + shapeB->m_radius;
    const float lengthSquare = glm::dot(ab, ab);
    if (lengthSquare <= radiusAB * radiusAB) return true;

    return false;
}
