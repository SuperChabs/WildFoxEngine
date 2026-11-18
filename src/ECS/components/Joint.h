#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>

struct BodyInfo;

enum class JointType {
    NONE,
    DISTANCE,
    ORIENTATION,
    CONSTANT_VELOCITY,
    CONSTANT_VELOCITY_LIMITED,
    HINGE,
    HINGE_LIMITED
};

constexpr const char *kJointTypeNames[] = {
    "NONE",
    "Distance",
    "Orientation",
    "Constant Velocity",
    "Constant Velocity (limited)",
    "Hinge",
    "Hinge (limited)"
};

struct c_Joint {
    JointType type = JointType::DISTANCE;

    entt::entity bodyA = entt::null;
    entt::entity bodyB = entt::null;

    glm::vec3 localAnchorA{0.0f};
    glm::vec3 localAnchorB{0.0f};
    glm::vec3 axisA{0.0f};
    glm::vec3 axisB{0.0f};

    glm::quat q0 = glm::identity<glm::quat>(); // the initial relative quaternion q1^-1 * q2
    bool q0Initialized = false;
};
