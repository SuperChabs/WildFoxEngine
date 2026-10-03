#pragma once

#include <glm/glm.hpp>

#include "ECS/components/Physics.h"

#define ND [[nodiscard]]

class Bounds;
struct Sphere;
struct Box;
struct ConvexHull;
//TODO: delete after full implementation of box collider
struct AABB;

namespace Shape {

    [[deprecated("There's no implemetation. Dont use ist")]]
    void Build(Sphere& s, const std::vector<glm::vec3> &pts, int num);
    void Build(Box& s, const std::vector<glm::vec3> &pts, int num);
    void Build(ConvexHull &s, const std::vector<glm::vec3> &pts, int num);
    //TODO: delete after full implementation of box collider
    void Build(AABB& s, const std::vector<glm::vec3> &pts, int num);

    ND glm::mat3 InertiaTensor(const Sphere& s);
    ND glm::mat3 InertiaTensor(const Box& s);
    ND glm::mat3 InertiaTensor(const ConvexHull& s);
    //TODO: delete after full implementation of box collider
    ND glm::mat3 InertiaTensor(const AABB& s);

    ND Bounds GetBounds(const Sphere& s, const glm::vec3 &pos, const glm::quat &rot);
    ND Bounds GetBounds(const Sphere& s);
    ND Bounds GetBounds(const Box& s, const glm::vec3 &pos, const glm::quat &rot);
    ND Bounds GetBounds(const Box& s);
    ND Bounds GetBounds(const ConvexHull& s, const glm::vec3 &pos, const glm::quat &rot);
    ND Bounds GetBounds(const ConvexHull& s);
    //TODO: delete after full implementation of box collider
    ND Bounds GetBounds(const AABB& s, const glm::vec3 &pos, const glm::quat &rot);
    ND Bounds GetBounds(const AABB& s);

    ND glm::vec3 Support(const Sphere& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, float bias);
    ND glm::vec3 Support(const Box& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, float bias);
    ND glm::vec3 Support(const ConvexHull& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, float bias);
    //TODO: delete after full implementation of box collider
    ND glm::vec3 Support(const AABB& s, const glm::vec3 &dir, const glm::vec3 &pos, const glm::quat &rot, float bias);

    ND float FastestLinearSpeed(const Sphere& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
    ND float FastestLinearSpeed(const Box& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
    ND float FastestLinearSpeed(const ConvexHull& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
    //TODO: delete after full implementation of box collider
    ND float FastestLinearSpeed(const AABB& s, const glm::vec3 &angularVelocity, const glm::vec3 &dir);
} // namespace Shape

//! ssf stands for "special shape functions"
namespace ssf {
    // Box

} // namespace SSF

#undef ND
