#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform 
{
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    mutable glm::mat4 modelMatrix;
    mutable bool isDirty;
    
    void UpdateModelMatrix() const;

public:
    Transform();
    Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl);
    
    // Position
    void SetPosition(const glm::vec3& pos);
    void Translate(const glm::vec3& translation);
    glm::vec3 GetPosition() const { return position; }
    
    // Rotation (в градусах)
    void SetRotation(const glm::vec3& rot);
    void Rotate(const glm::vec3& rotation);
    glm::vec3 GetRotation() const { return rotation; }
    
    // Scale
    void SetScale(const glm::vec3& scl);
    void SetScale(float uniformScale);
    glm::vec3 GetScale() const { return scale; }
    
    // Model Matrix
    const glm::mat4& GetModelMatrix() const;
    
    // Direction vectors
    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;
};

#endif