module;

#include <memory>

#include <glm/glm.hpp>
#include <glad/glad.h>

export module WFE.Rendering.RenderCommandBuilder;

import WFE.Rendering.Commands.RenderCommand;
import WFE.Rendering.Commands.DrawMeshCommand;
import WFE.Rendering.Commands.DrawSkyboxCommand;
import WFE.Rendering.Commands.ClearCommand;
import WFE.Rendering.MeshRenderer;
import WFE.Resource.Material.Material;
import WFE.Rendering.Core.GLContext;
import WFE.Rendering.RenderQueue;

export class RenderCommandBuilder
{
private:
    GLContext* context;

public:
    RenderCommandBuilder(GLContext* ctx) : context(ctx) {}
    
    std::unique_ptr<DrawMeshCommand> CreateDrawMesh(
        MeshRenderer& mesh,
        std::shared_ptr<Material> material,
        const glm::mat4& modelMatrix,
        uint32_t queueType = 1000) 
    {
        auto cmd = std::make_unique<DrawMeshCommand>(
            mesh, material, modelMatrix, context
        );
        
        cmd->sortKey = static_cast<uint64_t>(queueType) << 32;
        
        return cmd;
    }
    
    std::unique_ptr<DrawSkyboxCommand> CreateDrawSkybox(
        GLuint vao, 
        GLuint cubemap)
    {
        auto cmd = std::make_unique<DrawSkyboxCommand>(vao, cubemap, context);
        
        cmd->sortKey = static_cast<uint64_t>(RenderQueueType::BACKGROUND) << 32;
        
        return cmd;
    }
    
    std::unique_ptr<ClearCommand> CreateClear(
        const glm::vec4& color,
        bool clearColor = true,
        bool clearDepth = true,
        bool clearStencil = false)
    {
        return std::make_unique<ClearCommand>(
            color, clearColor, clearDepth, clearStencil, context
        );
    }
};