module;

#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module WFE.Rendering.Skybox;

import WFE.Core.Camera;

import WFE.Rendering.Core.VertexBuffer;
import WFE.Rendering.Core.VertexArray;

import WFE.Resource.Shader.ShaderManager;

export class Skybox 
{
private:
    std::unique_ptr<VertexArray> skyboxVAO;
    std::unique_ptr<VertexBuffer> skyboxVBO;

    unsigned int cubemapTexture;

    std::string shaderName;

    void SetupSkybox()
    {
        float skyboxVertices[] = 
        {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
        
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
        
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
        
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
        
            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
        
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };

        skyboxVAO = std::make_unique<VertexArray>();
        skyboxVBO = std::make_unique<VertexBuffer>();

        skyboxVBO->SetData(skyboxVertices, sizeof(skyboxVertices));
        skyboxVAO->Bind();
        skyboxVBO->Bind();
        skyboxVAO->AddAttribute(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
        skyboxVAO->Unbind();
    }

public:
    Skybox(unsigned int cubemapTex, const std::string& shaderName)
        : cubemapTexture(cubemapTex), shaderName(shaderName)
    {
        SetupSkybox();
    }

    //~Skybox() = default;

    void Render(ShaderManager& shaderManager, const Camera& camera, const glm::mat4& projection)
    {
        glDepthFunc(GL_LEQUAL);
        shaderManager.Bind(shaderName);

        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        shaderManager.SetMat4(shaderName, "view", view);
        shaderManager.SetMat4(shaderName, "projection", projection);

        skyboxVAO->Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        skyboxVAO->Unbind();

        glDepthFunc(GL_LESS);
    }

    void SetShader(const std::string& newShaderName) { shaderName = newShaderName; }
    std::string GetShader() const { return shaderName; }
    unsigned int GetTexture() const { return cubemapTexture; }
};
