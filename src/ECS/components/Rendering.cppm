module;

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module WFE.ECS.Components.Rendering;

import WFE.Scene.Mesh;
import WFE.Resource.Material.Material;
import WFE.Rendering.Primitive.PrimitivesFactory;

export struct MeshComponent 
{
    std::shared_ptr<Mesh> mesh;
    PrimitiveType type;
    
    MeshComponent() = default;
    MeshComponent(Mesh* m) : mesh(m) {}
    MeshComponent(Mesh* m, PrimitiveType t) : mesh(m), type(t) {}
    MeshComponent(std::shared_ptr<Mesh> m) : mesh(m) {}
};

export struct MaterialComponent 
{
    std::shared_ptr<Material> material;
    glm::vec2 tiling = {1.0f, 1.0f};
    
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