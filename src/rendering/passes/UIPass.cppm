module;

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

export module WFE.Rendering.Passes.UIPass;

import WFE.Rendering.Passes.RenderPass;
import WFE.Core.Camera;
import WFE.Core.CommandManager;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;

export class UIPass : public RenderPass
{
public:
    UIPass(GLContext* ctx, ShaderManager* sm)
        : RenderPass("UIPass", ctx, sm)
    {}
    
    void Setup() override
    {
        // Вмикаємо тест, щоб іконка ховалася за стінами
        context->SetDepthTest(true);
        // Використовуємо стандартну функцію порівняння
        context->SetDepthFunc(GL_LESS); 
        
        // Вимикаємо ЗАПИС у глибину для іконок (вони прозорі, це стандартна практика)
        glDepthMask(GL_FALSE);

        context->SetBlend(true);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void Execute(Camera& camera, const glm::mat4& projection) override
    {
        if (!enabled) return;
        
        Setup();
        
        CommandManager::ExecuteCommand("Renderer_RenderUI", 
        {
            &camera,
            projection,
            std::string("icon")
        });
        
        Cleanup();
    }
    
    void Cleanup() override
    {
        // Повертаємо запис глибини для наступного кадру
        glDepthMask(GL_TRUE);
        context->SetDepthTest(true);
        context->SetBlend(false);
    }
};