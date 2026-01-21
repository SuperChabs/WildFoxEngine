module;

#include <glad/glad.h>
#include <entt.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

export module WildFoxEngine.ECS.Systems;

import WildFoxEngine.ECS.Components; 
import WildFoxEngine.ECS.ECSWorld;

import WildFoxEngine.Core.Camera;
import WildFoxEngine.Core.Logger;

import WildFoxEngine.Resource.Shader.ShaderManager; 
import WildFoxEngine.Resource.Texture.TextureManager;

import WildFoxEngine.Rendering.Buffer;
import WildFoxEngine.Rendering.Light;

export class RenderSystem 
{
public:
    void Update(ECSWorld& world, ShaderManager& shaderManager, const std::string& name, Camera& camera) 
    {
        shaderManager.Bind(name);
        
        world.Each<TransformComponent, MeshComponent, MaterialComponent, VisibilityComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                MeshComponent& meshComp,
                MaterialComponent& matComp,
                VisibilityComponent& vis) 
        {
            if (!vis.isActive || !vis.visible) return;
            
            shaderManager.SetMat4(name, "model", transform.GetModelMatrix());
        
            if (matComp.material)
            {
                static int frameCount = 0;
                if (frameCount % 300 == 0)
                    Logger::Log(LogLevel::DEBUG, LogCategory::RENDERING, 
                        "Binding material: " + matComp.material->GetName());
                frameCount++;
                
                if (meshComp.mesh) 
                    meshComp.mesh->Draw(shaderManager, name, matComp.material);
            }
            else if (world.HasComponent<ColorComponent>(entity)) 
            {
                auto& color = world.GetComponent<ColorComponent>(entity);
                if (meshComp.mesh) 
                    meshComp.mesh->SetColor(color.color);
                
                if (meshComp.mesh)
                    meshComp.mesh->Draw(shaderManager, name);
            }
            else
            {
                if (meshComp.mesh)
                    meshComp.mesh->Draw(shaderManager, name);
            }
        });

        shaderManager.Unbind();
    }
};

export class RotationSystem 
{
public:
    void Update(ECSWorld& world, float deltaTime) 
    {
        world.Each<TransformComponent, RotationComponent>(
            [&](entt::entity entity, 
                TransformComponent& transform,
                RotationComponent& rotation) 
        {
            if (rotation.autoRotate) 
                transform.rotation += rotation.axis * rotation.speed * deltaTime;
        });
    }
};

export class LightSystem
{
public:
    void Update(ECSWorld& world, ShaderManager& shaderManager, const std::string& shaderName)
    {
        shaderManager.Bind(shaderName);
        
        int lightIndex = 0;
        const int maxLights = 8;
        
        world.Each<LightComponent, TransformComponent>(
            [&](entt::entity entity, LightComponent& light, TransformComponent& transform)
            {
                if (!light.isActive || lightIndex >= maxLights)
                    return;
                
                light.SyncWithTransform(transform);
                
                std::string base = "lights[" + std::to_string(lightIndex) + "]";
                
                shaderManager.SetInt(shaderName, base + ".type", static_cast<int>(light.type));
                
                shaderManager.SetVec3(shaderName, base + ".position", light.position);
                shaderManager.SetVec3(shaderName, base + ".direction", light.direction);
                
                shaderManager.SetVec3(shaderName, base + ".ambient", light.ambient * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".diffuse", light.diffuse * light.intensity);
                shaderManager.SetVec3(shaderName, base + ".specular", light.specular);
                
                if (light.type == LightType::POINT || light.type == LightType::SPOT)
                {
                    shaderManager.SetFloat(shaderName, base + ".constant", light.constant);
                    shaderManager.SetFloat(shaderName, base + ".linear", light.linear);
                    shaderManager.SetFloat(shaderName, base + ".quadratic", light.quadratic);
                }
                
                if (light.type == LightType::SPOT)
                {
                    shaderManager.SetFloat(shaderName, base + ".innerCutoff", 
                        glm::cos(glm::radians(light.innerCutoff)));
                    shaderManager.SetFloat(shaderName, base + ".outerCutoff", 
                        glm::cos(glm::radians(light.outerCutoff)));
                }
                
                lightIndex++;
            });

        shaderManager.SetInt(shaderName, "numLights", lightIndex);
        
        shaderManager.Unbind();
    }
};

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