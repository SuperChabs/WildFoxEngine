module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

export module WFE.Rendering.Grid;

import WFE.Rendering.Core.VertexArray;
import WFE.Rendering.Core.VertexBuffer;
import WFE.Rendering.Core.GLContext;
import WFE.Resource.Shader.ShaderManager;
import WFE.Core.Camera;
import WFE.Core.Logger;

export class Grid
{
private:
    std::unique_ptr<VertexArray> gridVAO;
    std::unique_ptr<VertexBuffer> gridVBO;
    
    bool enabled = true;
    
    float gridScale = 1.0f;
    float gridFadeDistance = 100.0f;
    float gridFadeStart = 50.0f;

    glm::vec3 gridColorThin = glm::vec3(0.3f, 0.3f, 0.3f);
    glm::vec3 gridColorThick = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 axisXColor = glm::vec3(1.0f, 0.3f, 0.3f);
    glm::vec3 axisZColor = glm::vec3(0.3f, 0.3f, 1.0f);
    
    float lineThickness = 1.0f;

public:
    Grid()
    {
        CreateQuad();
        Logger::Log(LogLevel::INFO, "Infinite Grid created");
    }
    
    void Render(ShaderManager& sm, const Camera& c, const glm::mat4& projection)
    {
        if (!enabled) return;
        
        sm.Bind("grid");
        
        glm::mat4 view = c.GetViewMatrix();
        
        sm.SetMat4("grid", "projection", projection);
        sm.SetMat4("grid", "view", view);
        
        sm.SetVec3("grid", "cPos", c.GetPosition());
        
        sm.SetFloat("grid", "gridScale", gridScale);
        sm.SetFloat("grid", "gridFadeDistance", gridFadeDistance);
        sm.SetFloat("grid", "gridFadeStart", gridFadeStart);
        
        sm.SetVec3("grid", "gridColorThin", gridColorThin);
        sm.SetVec3("grid", "gridColorThick", gridColorThick);
        sm.SetVec3("grid", "axisXColor", axisXColor);
        sm.SetVec3("grid", "axisZColor", axisZColor);
        
        sm.SetFloat("grid", "lineThickness", lineThickness);     

        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        gridVAO->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        gridVAO->Unbind();
        
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        
        sm.Unbind();
    }
    
    void SetEnabled(bool enable) { enabled = enable; }
    bool IsEnabled() const { return enabled; }
    
    void SetGridScale(float scale) { gridScale = scale; }
    float GetGridScale() const { return gridScale; }
    
    void SetFadeDistance(float distance) { gridFadeDistance = distance; }
    float GetFadeDistance() const { return gridFadeDistance; }
    
    void SetFadeStart(float start) { gridFadeStart = start; }
    float GetFadeStart() const { return gridFadeStart; }
    
    void SetThinLineColor(const glm::vec3& color) { gridColorThin = color; }
    void SetThickLineColor(const glm::vec3& color) { gridColorThick = color; }
    void SetAxisXColor(const glm::vec3& color) { axisXColor = color; }
    void SetAxisZColor(const glm::vec3& color) { axisZColor = color; }
    
    void SetLineThickness(float thickness) { lineThickness = thickness; }
    float GetLineThickness() const { return lineThickness; }

private:
    void CreateQuad()
    {
        float vertices[] = 
        {
            // Triangle 1
            -1.0f, 0.0f, -1.0f,
             1.0f, 0.0f, -1.0f,
             1.0f, 0.0f,  1.0f,
            // Triangle 2
             1.0f, 0.0f,  1.0f,
            -1.0f, 0.0f,  1.0f,
            -1.0f, 0.0f, -1.0f
        };
        
        gridVAO = std::make_unique<VertexArray>();
        gridVBO = std::make_unique<VertexBuffer>();
        
        gridVBO->SetData(vertices, sizeof(vertices), GL_STATIC_DRAW);
        
        gridVAO->Bind();
        gridVBO->Bind();
        
        gridVAO->AddAttribute(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
        
        gridVAO->Unbind();
    }
};