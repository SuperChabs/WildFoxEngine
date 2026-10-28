#include "Physics.h"

#include <cmath>
#include <variant>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "core/CommandManager.h"
#include "math/Math.h"
#include "shapeFunc/GJK.h"
#include "shapeFunc/ShapeFunctions.h"

Physics::Physics(ECSWorld &world) : world(world){
}

void Physics::Simulate(const float &dt) {
    entities.clear();

    // integration: gravity + position update for dynamic bodies
    world.Each<RigidBodyComponent, ColliderComponent>(
        [&](const entt::entity entity,
            RigidBodyComponent &r,
            ColliderComponent &c) {
            if (r.m_invMass != 0) {
                float mass = 1 / r.m_invMass;
                glm::vec3 impulseGravity = m_gravity * mass * dt;
                ApplyImpulseLinear(r, impulseGravity);
            }

            entities.push_back(entity);
        });

    //
    // BroadPhase
    //
    CollisionPairs collisionPairs;
    BroadPhase(entities, collisionPairs, static_cast<int>(entities.size()), dt);

    //
    // NarrowPhase
    //

    std::set<std::pair<entt::entity, entt::entity> > currentTriggers;

    int numContacts = 0;
    std::vector<ContactInfo> contacts;
    contacts.reserve(entities.size() * entities.size());
    for (const auto [fst, snd] : collisionPairs) {
        const auto &col_a = world.GetComponent<ColliderComponent>(fst);
        const auto &col_b = world.GetComponent<ColliderComponent>(snd);
        const auto &r_a = world.GetComponent<RigidBodyComponent>(fst);
        const auto &r_b = world.GetComponent<RigidBodyComponent>(snd);

        if (r_a.m_invMass == 0.0f && r_b.m_invMass == 0.0f) continue;

        if (ContactInfo contact{}; Intersect(fst, snd, dt, contact)) {
            if (col_a.isTrigger || col_b.isTrigger) {
                currentTriggers.insert({fst, snd});
            } else {
                contacts.push_back(contact);
                numContacts++;
            }
        }
    }

    if (numContacts > 1)
        std::ranges::sort(contacts,
                          [](const ContactInfo &a, const ContactInfo &b) {
                              return a.timeOfImpact < b.timeOfImpact;
                          });

    float accumulatedTime = 0.0f;
    for (auto &contact : contacts) {
        const float dtStep = contact.timeOfImpact - accumulatedTime;

        const auto &r_a = world.GetComponent<RigidBodyComponent>(contact.a);
        const auto &r_b = world.GetComponent<RigidBodyComponent>(contact.b);
        if (r_a.m_invMass == 0.0f && r_b.m_invMass == 0.0f) continue;

        for (const auto &e : entities)
            IntegrateBody(e, dtStep);

        ResolveContact(contact);
        accumulatedTime += dtStep;
    }

    const float timeRemaining = dt - accumulatedTime;
    if (timeRemaining > 0.0f) {
        for (const auto entity : entities)
            IntegrateBody(entity, timeRemaining);
    }

    // trigger enter/exit diffing vs last frame
    for (auto &p: currentTriggers)
        if (!m_activeTriggers.contains(p)) {
            CommandManager::ExecuteCommand("OnTriggerEnter", {p.first, p.second});
            Logger::Log(LogLevel::INFO, "Enter trigger");
        }

    for (auto &p: m_activeTriggers)
        if (!currentTriggers.contains(p)) {
            CommandManager::ExecuteCommand("OnTriggerExit", {p.first, p.second});
            Logger::Log(LogLevel::INFO, "Exit trigger");
        }

    m_activeTriggers = currentTriggers;

    static int s_debugLines = 0;
    if (numContacts > 0 && s_debugLines < 60) {
        for (const auto e : entities) {
            const auto &r = world.GetComponent<RigidBodyComponent>(e);
            if (r.m_invMass == 0.0f) continue;
            const auto &t = world.GetComponent<TransformComponent>(e);

            Logger::Log(LogLevel::DEBUG, std::format(
                "y={:.3f} vy={:.3f} w=({:.2f},{:.2f},{:.2f}) contacts={}",
                t.position.y, r.m_linearVelocity.y,
                r.m_angularVelocity.x, r.m_angularVelocity.y, r.m_angularVelocity.z,
                numContacts));
            ++s_debugLines;
        }
    }
}

bool Physics::Intersect(entt::entity entityA, entt::entity entityB, float dt, ContactInfo &contact) const {
    contact.a = entityA;
    contact.b = entityB;

    const auto &colA = world.GetComponent<ColliderComponent>(entityA);
    const auto &colB = world.GetComponent<ColliderComponent>(entityB);
    const auto &t_a = world.GetComponent<TransformComponent>(entityA);
    const auto &t_b = world.GetComponent<TransformComponent>(entityB);

    const auto isUnitSphere = [](const ColliderComponent &collider, const TransformComponent &transform) {
        if (!std::holds_alternative<Sphere>(collider.shape))
            return false;

        const Sphere &sphere = std::get<Sphere>(collider.shape);
        return sphere.m_center == glm::vec3(0.0f)
            && transform.scale == glm::vec3(1.0f);
    };
    const bool bothSpheres = isUnitSphere(colA, t_a) && isUnitSphere(colB, t_b);

    if (!bothSpheres)
        return ConservativeAdvance(entityA, entityB, dt, contact); // GJK

    const auto &sphereA = std::get<Sphere>(colA.shape);
    const auto &sphereB = std::get<Sphere>(colB.shape);

    const auto &r_a = world.GetComponent<RigidBodyComponent>(entityA);
    const auto &r_b = world.GetComponent<RigidBodyComponent>(entityB);

    if (!SphereSphereDynamic(&sphereA, &sphereB, t_a.position, t_b.position,
                             r_a.m_linearVelocity, r_b.m_linearVelocity, dt,
                             contact.ptOnA_WorldSpace, contact.ptOnB_WorldSpace,
                             contact.timeOfImpact))
        return false;

    // Step bodies forward to get local space collision points
    IntegrateBody(entityA, contact.timeOfImpact);
    IntegrateBody(entityB, contact.timeOfImpact);

    // Convert world space contacts to local space
    contact.ptOnA_LocalSpace = WorldSpaceToBodySpace(entityA, contact.ptOnA_WorldSpace);
    contact.ptOnB_LocalSpace = WorldSpaceToBodySpace(entityB, contact.ptOnB_WorldSpace);

    contact.normal = glm::normalize(t_a.position - t_b.position);

    // Unwind time step
    IntegrateBody(entityA, -contact.timeOfImpact);
    IntegrateBody(entityB, -contact.timeOfImpact);

    // Calculate the separation distance
    const glm::vec3 ab = t_b.position - t_a.position;
    contact.separationDistance = glm::length(ab) - (sphereA.m_radius + sphereB.m_radius);

    return true;
}

bool Physics::Intersect(const entt::entity entityA, const entt::entity entityB, ContactInfo &contact) const {
    contact.a = entityA;
    contact.b = entityB;
    contact.timeOfImpact = 0.0f;

    const auto &colA = world.GetComponent<ColliderComponent>(entityA);
    const auto &colB = world.GetComponent<ColliderComponent>(entityB);
    const auto &t_a = world.GetComponent<TransformComponent>(entityA);
    const auto &t_b = world.GetComponent<TransformComponent>(entityB);

    const auto isUnitSphere = [](const ColliderComponent &collider, const TransformComponent &transform) {
        if (!std::holds_alternative<Sphere>(collider.shape))
            return false;

        const Sphere &sphere = std::get<Sphere>(collider.shape);
        return sphere.m_center == glm::vec3(0.0f)
            && transform.scale == glm::vec3(1.0f);
    };
    if (isUnitSphere(colA, t_a) && isUnitSphere(colB, t_b)) {
        const auto &sphereA = std::get<Sphere>(colA.shape);
        const auto &sphereB = std::get<Sphere>(colB.shape);

        const glm::vec3 posA = t_a.position;
        const glm::vec3 posB = t_b.position;

        if (SphereSphereStatic(&sphereA, &sphereB, posA, posB, contact.ptOnA_WorldSpace, contact.ptOnB_WorldSpace)) {
            contact.normal = posA - posB;
            contact.normal = glm::normalize(contact.normal);

            contact.ptOnA_LocalSpace = WorldSpaceToBodySpace(entityA, contact.ptOnA_WorldSpace);
            contact.ptOnB_LocalSpace = WorldSpaceToBodySpace(entityB, contact.ptOnB_WorldSpace);

            glm::vec3 ab = t_b.position - t_a.position;
            float r = math::GetMagnitude(ab) - (sphereA.m_radius + sphereB.m_radius);
            contact.separationDistance = r;
            return true;
        }
    }
    else {
        glm::vec3 ptOnA;
        glm::vec3 ptOnB;
        constexpr float bias = 0.001f;
        if (GJK::DoesIntersect(world, entityA, entityB, bias, ptOnA, ptOnB)) {
            // there was an intersection, so get the contact data
            glm::vec3 normal = ptOnB - ptOnA;
            normal = glm::normalize(normal);

            ptOnA -= normal * bias;
            ptOnB += normal * bias;

            contact.normal = normal;

            contact.ptOnA_WorldSpace = ptOnA;
            contact.ptOnB_WorldSpace = ptOnB;

            contact.ptOnA_LocalSpace = WorldSpaceToBodySpace(entityA, contact.ptOnA_WorldSpace);
            contact.ptOnB_LocalSpace = WorldSpaceToBodySpace(entityB, contact.ptOnB_WorldSpace);

            const float r = math::GetMagnitude(ptOnA - ptOnB);
            contact.separationDistance = -r;
            return true;
        }

        // there was no collision, but we still want the contact data, so get it
        GJK::CLosestPoints(world, entityA, entityB, ptOnA, ptOnB);
        contact.ptOnA_WorldSpace = ptOnA;
        contact.ptOnB_WorldSpace = ptOnB;

        contact.ptOnA_LocalSpace = WorldSpaceToBodySpace(entityA, contact.ptOnA_WorldSpace);
        contact.ptOnB_LocalSpace = WorldSpaceToBodySpace(entityB, contact.ptOnB_WorldSpace);

        const float r = math::GetMagnitude(ptOnA - ptOnB);
        contact.separationDistance = r;
    }

    return false;
}

bool Physics::ConservativeAdvance(const entt::entity entityA, const entt::entity entityB, float dt, ContactInfo &contact) const {
    contact.a = entityA;
    contact.b = entityB;

    float toi = 0.0f;

    int numIters = 0;

    const auto &c_a = world.GetComponent<ColliderComponent>(entityA);
    const auto &c_b = world.GetComponent<ColliderComponent>(entityB);
    const auto &r_a = world.GetComponent<RigidBodyComponent>(entityA);
    const auto &r_b = world.GetComponent<RigidBodyComponent>(entityB);

    // advance the positions of the bodies until they touch or there's not time left
    while (dt > 0.0f) {
        // check for intersection
        if (Intersect(entityA, entityB, contact)) {
            contact.timeOfImpact = toi;
            IntegrateBody(entityA, -toi);
            IntegrateBody(entityB, -toi);
            return true;
        }

        ++numIters;
        if (numIters > 10) break;

        // get the vector from the closest point on A to the closest point on B
        glm::vec3 ab = contact.ptOnB_WorldSpace - contact.ptOnA_WorldSpace;
        ab = glm::normalize(ab);

        // project the relative velocity onto the ray of shortest distance
        glm::vec3 relativeVelocity = r_a.m_linearVelocity - r_b.m_linearVelocity;
        float orthoSpeed = glm::dot(ab, relativeVelocity);

        // add to the orthoSpeed the maximum angular speeds of the relative shapes
        float angularSpeedA = c_a.FastestLinearSpeed(r_a.m_angularVelocity, ab);
        float angularSpeedB = c_b.FastestLinearSpeed(r_b.m_angularVelocity, ab) * -1.0f;
        orthoSpeed += angularSpeedA + angularSpeedB;
        if (orthoSpeed <= 0.0f) break;

        float timeToGo = contact.separationDistance / orthoSpeed;
        if (timeToGo > dt) break;

        dt -= timeToGo;
        toi += timeToGo;
        IntegrateBody(entityA, timeToGo);
        IntegrateBody(entityB, timeToGo);
    }

    // unwind the clock
    IntegrateBody(entityA, -toi);
    IntegrateBody(entityB, -toi);
    return false;
}

bool Physics::RaySphere(const glm::vec3 &rayStart, const glm::vec3 &rayDir, const glm::vec3 &sphereCenter,
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

bool Physics::SphereSphereDynamic(const Sphere *shapeA, const Sphere *shapeB, const glm::vec3 posA, const glm::vec3 posB,
                                  const glm::vec3 &velA, const glm::vec3 &velB, const float dt, glm::vec3 &ptOnA, glm::vec3 &ptOnB, float &toi) {
    const glm::vec3 relativeVelocity = velA - velB;

    const glm::vec3 startPtA = posA;
    const glm::vec3 endPtA = posA + relativeVelocity * dt;
    const glm::vec3 rayDir = endPtA - startPtA;

    float t0 = 0, t1 = 0;
    if (glm::length(rayDir) < 0.001f * 0.001f) {
        // ray is too short, js chek if already intersecting
        glm::vec3 ab = posB - posA;
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

bool Physics::SphereSphereStatic(const Sphere *shapeA, const Sphere *shapeB, glm::vec3 posA, glm::vec3 posB,
    glm::vec3 &ptOnA, glm::vec3 &ptOnB) {
    const glm::vec3 ab = posB - posA;
    glm::vec3 norm = ab;
    norm = glm::normalize(norm);

    ptOnA = posA + norm * shapeA->m_radius;
    ptOnB = posB - norm * shapeB->m_radius;

    const float radiusAB = shapeA->m_radius + shapeB->m_radius;
    const float lengthSquare = glm::dot(ab, ab);
    if (lengthSquare <= radiusAB * radiusAB) return true;

    return false;
}

void Physics::ResolveContact(const ContactInfo &contact) const {
    auto &rb_a = world.GetComponent<RigidBodyComponent>(contact.a);
    auto &rb_b = world.GetComponent<RigidBodyComponent>(contact.b);
    auto &col_a = world.GetComponent<ColliderComponent>(contact.a);
    auto &col_b = world.GetComponent<ColliderComponent>(contact.b);
    auto &t_a = world.GetComponent<TransformComponent>(contact.a);
    auto &t_b = world.GetComponent<TransformComponent>(contact.b);

    const glm::vec3 comA = GetCenterOfMassWorldSpace(col_a, t_a);
    const glm::vec3 comB = GetCenterOfMassWorldSpace(col_b, t_b);

    const float elasticityA = rb_a.m_elasticity;
    const float elasticityB = rb_b.m_elasticity;
    const float elasticityTotal = elasticityA * elasticityB;

    const float invMassA = rb_a.m_invMass;
    const float invMassB = rb_b.m_invMass;

    // resolution: inelastic, linear-only impulse
    const float total = invMassA + invMassB;
    if (total == 0.0f) return;

    const glm::mat3 invWorldInertiaA = GetInverseInertiaTensorWorldSpace(col_a.GetInertiaTensor(), rb_a.m_invMass,
                                                                         t_a.rotation);
    const glm::mat3 invWorldInertiaB = GetInverseInertiaTensorWorldSpace(col_b.GetInertiaTensor(), rb_b.m_invMass,
                                                                         t_b.rotation);

    const glm::vec3 &n = contact.normal;

    Logger::Log(LogLevel::DEBUG, std::format(
        "n=({:.3f},{:.3f},{:.3f}) depth={:.4f} toi={:.5f}",
        n.x, n.y, n.z, -contact.separationDistance, contact.timeOfImpact));

    const glm::vec3 ra = contact.ptOnA_WorldSpace - comA;
    const glm::vec3 rb = contact.ptOnB_WorldSpace - comB;

    const glm::vec3 angularJA = glm::cross(invWorldInertiaA * glm::cross(ra, n), ra);
    const glm::vec3 angularJB = glm::cross(invWorldInertiaB * glm::cross(rb, n), rb);
    const float angularFactor = glm::dot(n, angularJA + angularJB);

    // get the world space velocity of motion and rotation
    const glm::vec3 velA = rb_a.m_linearVelocity + glm::cross(rb_a.m_angularVelocity, ra);
    const glm::vec3 velB = rb_b.m_linearVelocity + glm::cross(rb_b.m_angularVelocity, rb);

    // calculate the collision impulse
    const glm::vec3 vab = velA - velB;
    const float impulseJ = (1.0f + elasticityTotal) * glm::dot(n, vab) / (total + angularFactor);
    const glm::vec3 vectorImpulseJ = n * impulseJ;

    ApplyImpulse(rb_a, col_a, t_a, contact.ptOnA_WorldSpace, vectorImpulseJ * -1.0f);
    ApplyImpulse(rb_b, col_b, t_b, contact.ptOnB_WorldSpace, vectorImpulseJ * 1.0f);

    //
    // calculate the impulse caused by friction
    //

    const float frictionA = rb_a.m_friction;
    const float frictionB = rb_b.m_friction;
    const float frictionTotal = frictionA * frictionB;

    // find the normal direction of the velocity with respect to the normal of the collision
    const glm::vec3 velNorm = n * glm::dot(vab, n);

    // find the normal direction of the velocity with respect to the normal of the collision
    const glm::vec3 velTang = vab - velNorm;

    const float tangentLenSq = glm::dot(velTang, velTang);
    if (tangentLenSq > 1e-8f) {
        // get the tangential velocities relative to the other body
        glm::vec3 relativeVelTang = glm::normalize(velTang);

        const glm::vec3 inertiaA = glm::cross(invWorldInertiaA * glm::cross(ra, relativeVelTang), ra);
        const glm::vec3 inertiaB = glm::cross(invWorldInertiaB * glm::cross(rb, relativeVelTang), rb);
        const float invInertia = glm::dot(relativeVelTang, inertiaA + inertiaB);

        // calculate the tangential impuls for friction
        const glm::vec3 tdir = glm::normalize(velTang);
        float jt = glm::length(velTang) / (total + invInertia);
        jt = std::min(jt, frictionTotal * std::abs(impulseJ));
        const glm::vec3 impulseFriction = tdir * jt;

        // apply kinetic friction
        ApplyImpulse(rb_a, col_a, t_a, contact.ptOnA_WorldSpace, impulseFriction * -1.0f);
        ApplyImpulse(rb_b, col_b, t_b, contact.ptOnB_WorldSpace, impulseFriction * 1.0f);
    }

    //
    // positional correction
    //

    constexpr float slop = 0.01f;
    constexpr float percent = 0.4f; // 0.2-0.8
    const float depth = -contact.separationDistance;
    const float correction = std::max(depth - slop, 0.0f) * percent;

    const float tA = invMassA / total;
    const float tB = invMassB / total;
    const glm::vec3 ds = contact.normal * correction;
    t_a.position += contact.normal * correction * tA;
    t_b.position -= contact.normal * correction * tB;

    const glm::vec3 va = rb_a.m_linearVelocity + glm::cross(rb_a.m_angularVelocity, ra);
    const glm::vec3 vb = rb_b.m_linearVelocity + glm::cross(rb_b.m_angularVelocity, rb);
    Logger::Log(LogLevel::INFO, "post-impulse vn = " + std::to_string(glm::dot(n, va - vb)));
}

void Physics::IntegrateBody(entt::entity entity, float dt) const {
    auto &t = world.GetComponent<TransformComponent>(entity);
    auto &r = world.GetComponent<RigidBodyComponent>(entity);
    const auto &c = world.GetComponent<ColliderComponent>(entity);

    t.position += r.m_linearVelocity * dt;

    const glm::vec3 positionCM = GetCenterOfMassWorldSpace(c, t);
    const glm::vec3 cmToPos = t.position - positionCM;

    const glm::mat3 orientation = glm::mat3_cast(t.rotation);
    const glm::mat3 inertiaTensor = orientation * c.GetInertiaTensor() * glm::transpose(orientation);
    const glm::vec3 alpha = glm::inverse(inertiaTensor) * (-glm::cross(r.m_angularVelocity, inertiaTensor * r.m_angularVelocity));
    r.m_angularVelocity += alpha * dt;

    const glm::vec3 dAngle = r.m_angularVelocity * dt;
    const float angle = glm::length(dAngle);

    glm::quat dq(1.0f, 0.0f, 0.0f, 0.0f);
    if (angle > 1e-8f) {
        dq = glm::angleAxis(angle, dAngle / angle);
    }

    t.rotation = glm::normalize(dq * t.rotation);
    t.position = positionCM + glm::rotate(dq, cmToPos);
}

void Physics::ApplyImpulse(::RigidBodyComponent &rb, const ::ColliderComponent &c,
                           const ::TransformComponent &t, const glm::vec3 &impulsePoint, const glm::vec3 & impulse) {
    if (rb.m_invMass == 0.0f) return;

    // impulsePoint is the world space location of the application of the impulse
    // impulse is the world space direction and magnitude of the impulse
    ApplyImpulseLinear(rb, impulse);

    const glm::vec3 position = GetCenterOfMassWorldSpace(c, t); // applying impulses must produce torques through the center of mass
    const glm::vec3 r = impulsePoint - position;
    const glm::vec3 dL = glm::cross(r, impulse); // this is in world space
    ApplyImpulseAngular(rb, c, t, dL);
}

void Physics::ApplyImpulseLinear(RigidBodyComponent &rb, const glm::vec3 &impulse) {
    rb.m_linearVelocity += impulse * rb.m_invMass;
}

void Physics::ApplyImpulseAngular(RigidBodyComponent &rb, const ColliderComponent &c,
                                  const TransformComponent &t, const glm::vec3 &impulse) {
    if (rb.m_invMass == 0.0f) return;

    rb.m_angularVelocity +=
        GetInverseInertiaTensorWorldSpace(c.GetInertiaTensor(), rb.m_invMass, t.rotation) * impulse;

    constexpr float maxAngularSpeed = 30.0f;
    if (glm::length(rb.m_angularVelocity) > maxAngularSpeed)
        rb.m_angularVelocity = glm::normalize(rb.m_angularVelocity) * maxAngularSpeed;
}

void Physics::SortBodiesBounds(const std::vector<entt::entity> &entities, const int num, std::vector<PseudoBody> &sortedArray,
                               float dt) const {
    auto axis = glm::vec3(1.0f);
    axis = glm::normalize(axis);

    for (int i = 0; i < num; i++) {
        const auto &entity = entities[i];
        auto &col = world.GetComponent<ColliderComponent>(entity);
        const auto &trans = world.GetComponent<TransformComponent>(entity);
        const auto &rig = world.GetComponent<RigidBodyComponent>(entity);

        Bounds bounds = col.GetBounds(trans.position, trans.rotation, trans.scale);

        // expand the bounds by the linear velocity
        bounds.Expand(bounds.GetMins() + rig.m_linearVelocity * dt);
        bounds.Expand(bounds.GetMaxs() + rig.m_linearVelocity * dt);

        constexpr float epsilon = 0.1f;
        bounds.Expand(bounds.GetMins() + glm::vec3(-1.0f) * epsilon);
        bounds.Expand(bounds.GetMaxs() + glm::vec3(1.0f) * epsilon);

        sortedArray[i * 2].id = entities[i];
        sortedArray[i * 2].value = glm::dot(axis, bounds.GetMins());
        sortedArray[i * 2].isMin = true;

        sortedArray[i * 2 + 1].id = entities[i];
        sortedArray[i * 2 + 1].value = glm::dot(axis, bounds.GetMaxs());
        sortedArray[i * 2 + 1].isMin = false;
    }
    std::ranges::sort(sortedArray, {}, &PseudoBody::value);
}

void Physics::BuildPairs(CollisionPairs &collisionPairs,
                         const std::vector<PseudoBody> &sortedArray, const int num) {
    collisionPairs.clear();

    for (int i = 0; i < num * 2; i++) {
        const auto &a = sortedArray[i];
        if (!a.isMin)
            continue;

        const entt::entity fst = a.id;

        for (int j = i + 1; j < num * 2; j++) {
            const auto &b = sortedArray[j];
            if (b.id == fst) break;
            if (!b.isMin) continue;
            collisionPairs.emplace_back(fst, b.id);
        }
    }
}

void Physics::SweepAndPrune1D(const std::vector<entt::entity> &entities, CollisionPairs &finalPairs, const int num,
        const float dt) const {
    std::vector<PseudoBody> sortedBodies(num * 2);

    SortBodiesBounds(entities, num, sortedBodies, dt);
    BuildPairs(finalPairs, sortedBodies, num);
}

void Physics::BroadPhase(const std::vector<entt::entity> &entities, CollisionPairs &finalPairs, const int num, const float dt) const {
    SweepAndPrune1D(entities, finalPairs, num, dt);
}

glm::mat3 Physics::GetInverseInertiaTensorBodySpace(const glm::mat3 &inertia, const float &invMass) {
    return glm::inverse(inertia) * invMass;
}

glm::mat3 Physics::GetInverseInertiaTensorWorldSpace(const glm::mat3 &inertia, const float &invMass, const glm::quat &rotation) {
    glm::mat3 invInertiaTensor = glm::inverse(inertia) * invMass;
    const glm::mat3 rot = glm::mat3_cast(rotation);
    invInertiaTensor = rot * invInertiaTensor * glm::transpose(rot);
    return invInertiaTensor;
}

glm::vec3 Physics::WorldSpaceToBodySpace(const entt::entity e, const glm::vec3 &worldPt) const {
    const auto &t = world.GetComponent<TransformComponent>(e);
    const auto &c = world.GetComponent<ColliderComponent>(e);
    const glm::vec3 tmp = worldPt - GetCenterOfMassWorldSpace(c, t);
    return glm::inverse(t.rotation) * tmp;
}

glm::vec3 Physics::GetCenterOfMassWorldSpace(const ColliderComponent &c, const TransformComponent &t) {
    const glm::vec3 centerOfMass = std::visit([](const auto& shape) {
        return shape.m_centerOfMass;
    }, c.shape);
    const glm::vec3 pos = t.position + glm::rotate(t.rotation, centerOfMass * t.scale);
    return pos;
}
