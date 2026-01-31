module;

#include <vector>
#include <string>
#include <memory>

#include <entt.hpp>
#include <glm/glm.hpp>

export module WFE.Resource.Model.ModelNode;

export struct ModelNode
{
    std::string name;
    glm::mat4 localTransform;

    std::vector<int> meshIndices;
    std::vector<std::shared_ptr<ModelNode>> children;
    ModelNode* parent = nullptr;

    ModelNode() : localTransform(1.0f) {}
    ModelNode(const std::string& n, const glm::mat4& transform = glm::mat4(1.0f))
        : name(n), localTransform(transform) 
    {}

    void AddChild(std::shared_ptr<ModelNode> child)
    {
        child->parent = this;
        children.push_back(child);
    }

    glm::mat4 GetGlobalTransform() const
    {
        if (parent)
            return parent->GetGlobalTransform() * localTransform;
        return localTransform;
    }

    bool HasChildren() const { return !children.empty(); }
    bool HasMeshes()   const { return !meshIndices.empty(); } 
};
