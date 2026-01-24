module;

#include <memory>

#include <glm/glm.hpp>

export module WFE.ECS.Components.Rendering;

import WFE.Scene.Mesh;

import WFE.Resource.Material.Material;

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