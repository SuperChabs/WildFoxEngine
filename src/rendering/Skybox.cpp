#include "rendering/Skybox.h"
#include "utils/Logger.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

Skybox::Skybox(unsigned int cubemapTex, Shader* skyboxShader)
    : cubemapTexture(cubemapTex), shader(skyboxShader),
      skyboxVAO(nullptr), skyboxVBO(nullptr)
{
    SetupSkybox();

    Logger::Info("Skybox created with cubemap texture ID: " + std::to_string(cubemapTexture));
}

Skybox::~Skybox()
{
    delete skyboxVAO;
    delete skyboxVBO;
}

void Skybox::SetupSkybox()
{
    float skyboxVertices[] = 
    {
        // позиції
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
    
    skyboxVAO = new VertexArray();
    skyboxVBO = new VertexBuffer();
    
    skyboxVBO->SetData(skyboxVertices, sizeof(skyboxVertices));
    skyboxVAO->Bind();
    skyboxVBO->Bind();
    skyboxVAO->AddAttribute(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
    skyboxVAO->Unbind();

    Logger::Info("Skybox geometry setup completed");
}

void Skybox::Render(const Camera& camera, const glm::mat4& projection)
{
    glDepthFunc(GL_LEQUAL);
    
    shader->use();
    
    // Видаляємо трансляцію з view матриці
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