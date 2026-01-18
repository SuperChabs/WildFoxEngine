module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <memory>

export module XEngine.ECS.Components;

import XEngine.Scene.Mesh;

import XEngine.Rendering.Material;
import XEngine.Rendering.Light;
import XEngine.Rendering.MeshData;

export struct TagComponent 
{
    std::string name;
    
    TagComponent() = default;
    TagComponent(const std::string& n) : name(n) {}
};

export struct IDComponent 
{
    uint64_t id;
    
    IDComponent() = default;
    IDComponent(uint64_t i) : id(i) {}
};

export struct TransformComponent 
{
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
    
    TransformComponent() = default;
    TransformComponent(const glm::vec3& pos) : position(pos) {}
    TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
    
    glm::mat4 GetModelMatrix() const 
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, scale);
        return model;
    }
};

export struct MeshComponent 
{
    std::shared_ptr<Mesh> mesh;
    
    MeshComponent() = default;
    MeshComponent(Mesh* m) : mesh(m) {}
    MeshComponent(std::shared_ptr<Mesh> m) : mesh(m) {}
};

export struct MaterialComponent 
{
    std::shared_ptr<Material> material;
    
    MaterialComponent() = default;
    MaterialComponent(std::shared_ptr<Material> mat) : material(mat) {}
};

export struct ColorComponent 
{
    glm::vec3 color{1.0f};
    
    ColorComponent() = default;
    ColorComponent(const glm::vec3& col) : color(col) {}
    ColorComponent(float r, float g, float b) : color(r, g, b) {}
};

export struct VisibilityComponent 
{
    bool isActive = true;
    bool visible = true;
    
    VisibilityComponent() = default;
    VisibilityComponent(bool active) : isActive(active) {}
};

export struct RotationComponent 
{
    glm::vec3 axis{0, 0, 0};
    float speed = 50.0f;
    bool autoRotate = false;
    
    RotationComponent() = default;
    RotationComponent(float spd) : speed(spd), autoRotate(true) {}
};

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

export struct IconComponent 
{
    std::string iconTexturePath;
    unsigned int textureID = 0;
    float scale = 0.5f;
    bool billboardMode = true; 
    
    IconComponent() = default;
    IconComponent(const std::string& path, float s = 0.5f) 
        : iconTexturePath(path), scale(s) {}
};