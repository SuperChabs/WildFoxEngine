module;

#include <variant>
#include <glm/glm.hpp>

export module WFE.ECS.Components.Physics;

export struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

export struct Sphere //чіфівффів
{
    glm::vec3 centre;
    float radius;
};

export struct ColliderComponent
{
    std::variant<AABB, Sphere> shape;
    bool isTrigger = false;
};

export struct RigidBodyComponent
{
    float inv_mass;
    glm::vec3 velocity;
    glm::vec3 angular_velocity;
    glm::vec3 inertia;
    glm::vec3 force_accum;
    glm::vec3 torque_accum;
};