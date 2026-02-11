module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module WFE.ECS.Components.Lighting;

import WFE.Scene.Light;

import WFE.ECS.Components.Transform;

export struct LightComponent
{
    LightType type = LightType::DIRECTIONAL;
    
    glm::vec3 position{0.0f, 5.0f, 0.0f};
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
    
    glm::vec3 ambient{0.2f, 0.2f, 0.2f};
    glm::vec3 diffuse{0.8f, 0.8f, 0.8f};
    glm::vec3 specular{1.0f, 1.0f, 1.0f};
    
    float intensity = 1.0f;
    bool isActive = true;
    bool castShadows = true;
    
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float radius = 50.0f;
    
    float innerCutoff = 12.5f;
    float outerCutoff = 17.5f;
    
    LightComponent() = default;
    
    LightComponent(LightType t) : type(t)
    {
        if (t == LightType::DIRECTIONAL)
        {
            direction = glm::vec3(0.0f, -1.0f, 0.0f);
            ambient = glm::vec3(0.05f);
            diffuse = glm::vec3(0.4f);
            specular = glm::vec3(0.5f);
        }
        else if (t == LightType::POINT)
        {
            ambient = glm::vec3(0.05f);
            diffuse = glm::vec3(0.8f);
            specular = glm::vec3(1.0f);
        }
        else if (t == LightType::SPOT)
        {
            ambient = glm::vec3(0.0f);
            diffuse = glm::vec3(1.0f);
            specular = glm::vec3(1.0f);
        }
    }
    
    void SyncWithTransform(const TransformComponent& transform)
    {
        position = transform.position;
        
        glm::mat4 rotMat = glm::mat4(1.0f);
        rotMat = glm::rotate(rotMat, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
        rotMat = glm::rotate(rotMat, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
        rotMat = glm::rotate(rotMat, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
        
        glm::vec4 dir = rotMat * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        direction = glm::normalize(glm::vec3(dir));
    }
};