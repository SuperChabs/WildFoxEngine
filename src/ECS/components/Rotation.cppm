module;

#include <glm/glm.hpp>

export module WFE.ECS.Components.Rotation;

export struct RotationComponent 
{
    glm::vec3 axis{0, 0, 0};
    float speed = 50.0f;
    bool autoRotate = false;
    
    RotationComponent() = default;
    RotationComponent(float spd) : speed(spd), autoRotate(true) {}
};