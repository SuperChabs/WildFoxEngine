export module WFE.Rendering.Commands.DrawSkyboxCommand;

import WFE.Rendering.Commands.RenderCommand;
import WFE.Rendering.Core.GLContext;

export class DrawSkyboxCommand : public RenderCommand
{
public:
    GLuint vao;
    GLuint cubemap;
    GLContext* context;
    
    DrawSkyboxCommand(GLuint v, GLuint tex, GLContext* ctx)
        : RenderCommand(RenderCommandType::DRAW_SKYBOX)
        , vao(v)
        , cubemap(tex)
        , context(ctx)
    {}
    
    void Execute() override
    {
        if (!context) return;
        
        context->SetDepthFunc(GL_LEQUAL);
        context->BindVAO(vao);
        context->BindTexture2D(cubemap, 0);
        context->DrawArrays(GL_TRIANGLES, 0, 36);
        context->SetDepthFunc(GL_LESS);
    }
};