export module WFE.Rendering.Commands.ClearCommand;

import WFE.Rendering.Commands.RenderCommand;
import WFE.Rendering.Core.GLContext;

export class ClearCommand : public RenderCommand
{
public:
    glm::vec4 color;
    bool clearColor;
    bool clearDepth;
    bool clearStencil;
    GLContext* context;
    
    ClearCommand(const glm::vec4& c, bool color, bool depth, bool stencil, GLContext* ctx)
        : RenderCommand(RenderCommandType::CLEAR)
        , color(c)
        , clearColor(color)
        , clearDepth(depth)
        , clearStencil(stencil)
        , context(ctx)
    {}
    
    void Execute() override
    {
        if (!context) return;
        
        context->ClearColor(color.r, color.g, color.b, color.a);
        
        GLbitfield mask = 0;
        if (clearColor) mask |= GL_COLOR_BUFFER_BIT;
        if (clearDepth) mask |= GL_DEPTH_BUFFER_BIT;
        if (clearStencil) mask |= GL_STENCIL_BUFFER_BIT;
        
        context->Clear(mask);
    }
};