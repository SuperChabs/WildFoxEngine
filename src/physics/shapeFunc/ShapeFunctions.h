#pragma once

#include <glm/glm.hpp>

#include "ShapeFuncStruckt.h"
#include "ECS/World.h"
#include "ECS/components/Physics.h"

#define ND [[nodiscard]]

class Bounds;
struct Sphere;
struct Box;
struct ConvexHull;

namespace Shape {
    [[deprecated("There's no implemetation. Dont use ist")]]
    void Build(Sphere     &s, std::vector<glm::vec3> &pts, int num);
    void Build(Box        &s, std::vector<glm::vec3> &pts, int num);
    bool Build(ConvexHull *s, const std::vector<glm::vec3> &pts, int num, int maxVerts = 1024);

    ND glm::mat3 InertiaTensor(const Sphere& s);
    ND glm::mat3 InertiaTensor(const Box& s);
    ND glm::mat3 InertiaTensor(const ConvexHull& s);

    ND Bounds GetBounds(const Sphere& s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale);
    ND Bounds GetBounds(const Sphere& s);
    ND Bounds GetBounds(const Box& s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale);
    ND Bounds GetBounds(const Box& s);
    ND Bounds GetBounds(const ConvexHull& s, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale);
    ND Bounds GetBounds(const ConvexHull& s);

    ND glm::vec3 Support(const Sphere& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale, float bias);
    ND glm::vec3 Support(const Box& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3& scale, float bias);
    ND glm::vec3 Support(const ConvexHull& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, const glm::vec3 &scale, float bias);

    ND Point Support(ECSWorld &ecs, const entt::entity &bodyA, const entt::entity &bodyB, glm::vec3 dir, float bias);

    ND float FastestLinearSpeed(const Sphere& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
    ND float FastestLinearSpeed(const Box& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
    ND float FastestLinearSpeed(const ConvexHull& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
} // namespace Shape

#undef ND
