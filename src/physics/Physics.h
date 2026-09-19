#pragma once

#include <vector>
#include <set>
#include <utility>

#include <glm/glm.hpp>

#include "ECS/components/Components.h"
#include "ECS/World.h"
#include "PhysicsContext.h"

class Physics {
    glm::vec3 m_gravity = {0, 0, 0};
    std::vector<entt::entity> entities;

    std::set<std::pair<entt::entity, entt::entity> > m_activeTriggers;

    ECSWorld &world;

    using CollisionPairs = std::vector<std::pair<entt::entity, entt::entity>>;

public:
    explicit Physics(ECSWorld &world);

    void Simulate(const float &dt);

private:
    static bool TestAABB(const AABB &a, const AABB &b, ContactInfo &contact) ;
    bool TestSphere(const Sphere &a, const Sphere &b, ContactInfo &contact) const;

    bool Intersect(entt::entity entityA, entt::entity entityB, float dt, ContactInfo &contact) const;

    static bool RaySphere(const glm::vec3 &rayStart, const glm::vec3 &rayDir, const glm::vec3 &sphereCenter,
                      const float& sphereRadius, float &t1, float &t2);

    static bool SphereSphereDynamic(const Sphere *shapeA, const Sphere *shapeB, glm::vec3 posA, glm::vec3 posB,
                                    const glm::vec3 &velA, const glm::vec3 &velB, float dt, glm::vec3 &ptOnA, glm::vec3 &ptOnB, float &toi);

    void ResolveContact(const ContactInfo &contact) const;

    void IntegrateBody(entt::entity entity, float dt) const;

    static void ApplyImpulse(RigidBodyComponent &rb, const ColliderComponent &c,
                             const TransformComponent &t, const glm::vec3 & impulsePoint, const glm::vec3 & impulse);
    static void ApplyImpulseLinear(RigidBodyComponent &rb,  const glm::vec3 &impulse);
    static void ApplyImpulseAngular(RigidBodyComponent &rb, const ColliderComponent &c,
                                    const TransformComponent &t, const glm::vec3 &impulse);

    void SortBodiesBounds(const std::vector<entt::entity> &entities, int num, std::vector<PseudoBody> &sortedArray, float dt) const;

    static void BuildPairs(CollisionPairs &collisionPairs, const std::vector<PseudoBody> &sortedArray, int num);

    void SweepAndPrune1D(const std::vector<entt::entity> &entities, CollisionPairs &finalPairs, int num, float dt) const;

    void BroadPhase(const std::vector<entt::entity> &entities, CollisionPairs &finalPairs, int num, float dt) const;

    [[nodiscard]] static glm::mat3 GetInverseInertiaTensorBodySpace(const glm::mat3 &inertia, const float& invMass);
    [[nodiscard]] static glm::mat3 GetInverseInertiaTensorWorldSpace(const glm::mat3 &inertia, const float& invMass,
        const glm::quat &rotation);

    [[nodiscard]] glm::vec3 WorldSpaceToBodySpace(entt::entity e, const glm::vec3 &worldPt) const;

    static glm::vec3 GetCenterOfMassWorldSpace(const ColliderComponent &c, const TransformComponent &t);

    static glm::mat3 GetInertiaTensorFromShape(const ColliderComponent &c);
};
