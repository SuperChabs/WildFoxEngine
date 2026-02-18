module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

export module WFE.ECS.Components.Transform;

export struct TransformComponent 
{
    glm::vec3 position{0.0f};
    glm::quat rotation{glm::identity<glm::quat>()};  // ← кватерніон
    glm::vec3 scale{1.0f};
    
    TransformComponent() = default;
    TransformComponent(const glm::vec3& pos) : position(pos) {}
    TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
    
    glm::mat4 GetModelMatrix() const 
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, scale);
        return model;
    }

    glm::vec3 GetEulerDegrees() const
    {
        return glm::degrees(glm::eulerAngles(rotation));
    }
};