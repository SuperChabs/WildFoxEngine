#pragma once
#include <glm/vec3.hpp>
#include <entt/entt.hpp>

struct BodyInfo;

enum class JointType {
    Distance
};

struct c_Joint {
    JointType type = JointType::Distance;

    entt::entity bodyA = entt::null;
    entt::entity bodyB = entt::null;

    glm::vec3 localAnchorA{0.0f};
    glm::vec3 localAnchorB{0.0f};
    glm::vec3 axisA{0.0f};
    glm::vec3 axisB{0.0f};
};
