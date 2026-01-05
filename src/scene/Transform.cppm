module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module XEngine.Scene.Transform;

export class Transform 
{
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    mutable glm::mat4 modelMatrix;
    mutable bool isDirty;
    
    void UpdateModelMatrix() const
    {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, scale);
    }

public:
    Transform()
        : position(0.0f), rotation(0.0f), scale(1.0f), 
        modelMatrix(1.0f), isDirty(true)
    {}

    Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl), 
        modelMatrix(1.0f), isDirty(true)
    {}
    
    void SetPosition(const glm::vec3& pos)
    {
        position = pos;
        isDirty = true;
    }

    void Translate(const glm::vec3& translation)
    {
        position += translation;
        isDirty = true;
    }

    glm::vec3 GetPosition() const { return position; }
    
    void SetRotation(const glm::vec3& rot)
    {
        rotation = rot;
        isDirty = true;
    }

    void Rotate(const glm::vec3& rot)
    {
        rotation += rot;
        isDirty = true;
    }

    glm::vec3 GetRotation() const { return rotation; }
    
    void SetScale(const glm::vec3& scl)
    {
        scale = scl;
        isDirty = true;
    }

    void SetScale(float uniformScale)
    {
        scale = glm::vec3(uniformScale);
        isDirty = true;
    }

    glm::vec3 GetScale() const { return scale; }
    
    const glm::mat4& GetModelMatrix() const
    {
        if (isDirty)
        {
            UpdateModelMatrix();
            isDirty = false;
        }
        return modelMatrix;
    }
    
    glm::vec3 GetForward() const
    {
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    }

    glm::vec3 GetRight() const
    {
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
    }

    glm::vec3 GetUp() const
    {
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        return glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    }
};