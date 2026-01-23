export module WFE.Rendering.Commands.DrawMeshCommand;

import WFE.Rendering.Commands.RenderCommand;
import WFE.Rendering.MeshRenderer;
import WFE.Rendering.Material;
import WFE.Rendering.Core.GLContext;

export class DrawMeshCommand : public RenderCommand
{
public:
    GPUMesh* mesh;
    std::shared_ptr<Material> material;
    glm::mat4 modelMatrix;
    GLContext* context;
    
    DrawMeshCommand(GPUMesh* m, std::shared_ptr<Material> mat, 
                    const glm::mat4& model, GLContext* ctx)
        : RenderCommand(RenderCommandType::DRAW_MESH)
        , mesh(m)
        , material(mat)
        , modelMatrix(model)
        , context(ctx)
    {}
    
    void Execute() override
    {
        if (!mesh || !context) return;
        
        mesh->Draw();
    }
    
    void GenerateSortKey() override
    {
        uint64_t shaderHash = 0;
        
        uint64_t materialHash = material ? 
            (reinterpret_cast<uintptr_t>(material.get()) & 0xFFFFFF) : 0;

        uint32_t depth = static_cast<uint32_t>(
            (modelMatrix[3][2] + 100.0f) * 1000.0f
        );
        
        sortKey = (shaderHash << 56) | (materialHash << 32) | depth;
    }
};