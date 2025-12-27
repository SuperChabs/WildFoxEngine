#include "scene/Transform.h"

Transform::Transform()
    : position(0.0f), rotation(0.0f), scale(1.0f), 
      modelMatrix(1.0f), isDirty(true)
{
}

Transform::Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
    : position(pos), rotation(rot), scale(scl), 
      modelMatrix(1.0f), isDirty(true)
{
}

void Transform::SetPosition(const glm::vec3& pos)
{
    position = pos;
    isDirty = true;
}

void Transform::Translate(const glm::vec3& translation)
{
    position += translation;
    isDirty = true;
}

void Transform::SetRotation(const glm::vec3& rot)
{
    rotation = rot;
    isDirty = true;
}

void Transform::Rotate(const glm::vec3& rot)
{
    rotation += rot;
    isDirty = true;
}

void Transform::SetScale(const glm::vec3& scl)
{
    scale = scl;
    isDirty = true;
}

void Transform::SetScale(float uniformScale)
{
    scale = glm::vec3(uniformScale);
    isDirty = true;
}

const glm::mat4& Transform::GetModelMatrix() const
{
    if (isDirty)
    {
        UpdateModelMatrix();
        isDirty = false;
    }
    return modelMatrix;
}

void Transform::UpdateModelMatrix() const
{
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, scale);
}

glm::vec3 Transform::GetForward() const
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
}

glm::vec3 Transform::GetRight() const
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
}

glm::vec3 Transform::GetUp() const
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
}