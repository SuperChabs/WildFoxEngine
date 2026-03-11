module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

export module WFE.Rendering.Commands.DrawMeshCommand;

import WFE.Rendering.Commands.RenderCommand;
import WFE.Rendering.MeshRenderer;
import WFE.Resource.Material.Material;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

export class DrawMeshCommand : public RenderCommand
{
public:
    MeshRenderer& mesh;
    std::shared_ptr<Material> material;
    glm::mat4 modelMatrix;
    GLContext* context;
    ShaderManager* shaderManager;
    std::string shaderName;
    
    DrawMeshCommand(MeshRenderer& m, std::shared_ptr<Material> mat, 
                    const glm::mat4& model, GLContext* ctx,
                    ShaderManager* sm = nullptr, const std::string& shader = "basic")
        : RenderCommand(RenderCommandType::DRAW_MESH)
        , mesh(m)
        , material(mat)
        , modelMatrix(model)
        , context(ctx)
        , shaderManager(sm)
        , shaderName(shader)
    {}
    
    void Execute() override
    {
        if (!context) 
            return;
    
        if (shaderManager)
        {
            shaderManager->SetMat4(shaderName, "model", modelMatrix);
            
            if (material)
                material->Bind(*shaderManager, shaderName);
        }
        
        mesh.Draw();
        
        if (material && shaderManager)
            material->Unbind();
    }
    
    void GenerateSortKey() override
    {
        uint16_t queue = material && material->IsUsingColor() ? 1000 : 1000;
        
        uint32_t materialHash = material ? 
            static_cast<uint32_t>(reinterpret_cast<uintptr_t>(material.get()) & 0xFFFFFF) : 0;
        
        float zDepth = modelMatrix[3][2];
        uint32_t depth = static_cast<uint32_t>((zDepth + 100.0f) * 1000.0f) & 0xFFFFFF;
        
        sortKey = BuildSortKey(queue, materialHash, depth);
    }
};