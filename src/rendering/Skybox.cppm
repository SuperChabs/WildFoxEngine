module;

#include <vector>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module XEngine.Rendering.Skybox;

import XEngine.Core.Camera;
import XEngine.Core.Shader;
import XEngine.Rendering.Buffer; 

export class Skybox 
{
private:
    std::unique_ptr<VertexArray> skyboxVAO;
    std::unique_ptr<VertexBuffer> skyboxVBO;
    unsigned int cubemapTexture;
    Shader* shader;

    void SetupSkybox()
    {
        float skyboxVertices[] = 
        {
            -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
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
    Skybox(unsigned int cubemapTex, Shader* skyboxShader)
        : cubemapTexture(cubemapTex), shader(skyboxShader)
    {
        SetupSkybox();
    }

    //~Skybox() = default;

    void Render(const Camera& camera, const glm::mat4& projection)
    {
        glDepthFunc(GL_LEQUAL);
        shader->use();

        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        skyboxVAO->Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        skyboxVAO->Unbind();

        glDepthFunc(GL_LESS);
    }

    void SetShader(Shader* newShader) { shader = newShader; }
    Shader* GetShader() const { return shader; }
    unsigned int GetTexture() const { return cubemapTexture; }
};
