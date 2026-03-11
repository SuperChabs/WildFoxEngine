module;

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

export module WFE.Rendering.Commands.DrawIconCommand;

import WFE.Rendering.Commands.RenderCommand;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

export class DrawIconCommand : public RenderCommand
{
public:
    GLuint quadVAO;
    GLuint textureID;
    glm::mat4 modelMatrix;
    GLContext* context;
    ShaderManager* shaderManager;
    std::string shaderName;
    
    DrawIconCommand(GLuint vao, GLuint tex, const glm::mat4& model,
                    GLContext* ctx, ShaderManager* sm, const std::string& shader)
        : RenderCommand(RenderCommandType::DRAW_ICON)
        , quadVAO(vao)
        , textureID(tex)
        , modelMatrix(model)
        , context(ctx)
        , shaderManager(sm)
        , shaderName(shader)
    {}
    
    void Execute() override
    {
        if (!context || !shaderManager) return;
        
        // Set model matrix
        shaderManager->SetMat4(shaderName, "model", modelMatrix);
        
        // Bind texture
        context->BindTexture2D(textureID, 0);
        shaderManager->SetInt(shaderName, "iconTexture", 0);
        
        // Draw quad
        context->BindVAO(quadVAO);
        context->DrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    void GenerateSortKey() override
    {
        // Icons are always in overlay queue
        uint16_t queue = static_cast<uint16_t>(4000); // OVERLAY
        
        // Group by texture
        uint32_t textureHash = textureID & 0xFFFFFF;
        
        // Back to front sorting for transparency
        float zDepth = modelMatrix[3][2];
        uint32_t depth = static_cast<uint32_t>((100.0f - zDepth) * 1000.0f) & 0xFFFFFF;
        
        sortKey = BuildSortKey(queue, textureHash, depth);
    }
};