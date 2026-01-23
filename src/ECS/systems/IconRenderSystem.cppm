module;

#include <glad/glad.h>
#include <entt.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

export module WFE.ECS.Systems.IconRenderSystem;

import WFE.ECS.Components; 
import WFE.ECS.ECSWorld;

import WFE.Core.Camera;
import WFE.Core.Logger;

import WFE.Resource.Shader.ShaderManager; 
import WFE.Resource.Texture.TextureManager;

import WFE.Rendering.Core.VertexArray;
import WFE.Rendering.Core.VertexBuffer;

export class IconRenderSystem 
{
private:
    std::unique_ptr<VertexArray> quadVAO;
    std::unique_ptr<VertexBuffer> quadVBO;
    TextureManager* textureManager;
    
    void SetupQuad()
    {
        float quadVertices[] = {
            // positions   // texCoords
            -0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
             
            -0.5f,  0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f
        };
        
        quadVAO = std::make_unique<VertexArray>();
        quadVBO = std::make_unique<VertexBuffer>();
        
        quadVBO->SetData(quadVertices, sizeof(quadVertices));
        quadVAO->Bind();
        quadVBO->Bind();
        
        quadVAO->AddAttribute(0, 2, GL_FLOAT, false, 4 * sizeof(float), 0);
        quadVAO->AddAttribute(1, 2, GL_FLOAT, false, 4 * sizeof(float), 2 * sizeof(float));
        
        quadVAO->Unbind();
    }

public:
    IconRenderSystem(TextureManager* tm) : textureManager(tm)
    {
        SetupQuad();
    }
    
    void Update(ECSWorld& world, ShaderManager& shaderManager, 
                const std::string& shaderName, Camera& camera)
    {
        shaderManager.Bind(shaderName);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        
        world.Each<TransformComponent, IconComponent, VisibilityComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                IconComponent& icon,
                VisibilityComponent& vis) 
        {
            if (!vis.isActive || !vis.visible) return;
            
            if (icon.textureID == 0 && !icon.iconTexturePath.empty())
            {
                icon.textureID = textureManager->LoadTexture(icon.iconTexturePath);
                if (icon.textureID == 0)
                {
                    Logger::Log(LogLevel::WARNING, 
                        "Failed to load icon texture: " + icon.iconTexturePath);
                    return;
                }
            }
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);
            
            if (icon.billboardMode)
            {
                glm::mat4 viewMatrix = camera.GetViewMatrix();
                
                model[0][0] = viewMatrix[0][0];
                model[0][1] = viewMatrix[1][0];
                model[0][2] = viewMatrix[2][0];
                
                model[1][0] = viewMatrix[0][1];
                model[1][1] = viewMatrix[1][1];
                model[1][2] = viewMatrix[2][1];
                
                model[2][0] = viewMatrix[0][2];
                model[2][1] = viewMatrix[1][2];
                model[2][2] = viewMatrix[2][2];
            }
            else
            {
                model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
                model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
                model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
            }
            
            model = glm::scale(model, glm::vec3(icon.scale));
            
            shaderManager.SetMat4(shaderName, "model", model);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, icon.textureID);
            shaderManager.SetInt(shaderName, "iconTexture", 0);
            
            quadVAO->Bind();
            glDrawArrays(GL_TRIANGLES, 0, 6);
            quadVAO->Unbind();
        });
        
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
        shaderManager.Unbind();
    }
};