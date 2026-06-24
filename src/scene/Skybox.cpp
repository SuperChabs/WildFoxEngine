#include "Skybox.h"

void Skybox::SetupSkybox() {
    float skyboxVertices[] =
    {
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    skyboxVAO = std::make_unique<VertexArray>();
    skyboxVBO = std::make_unique<VertexBuffer>();

    skyboxVBO->SetData(skyboxVertices, sizeof(skyboxVertices));
    skyboxVAO->Bind();
    skyboxVBO->Bind();
    skyboxVAO->AddAttribute(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);
    skyboxVAO->Unbind();
}

Skybox::Skybox(unsigned int cubemapTex, const std::string &shaderName)
    : cubemapTexture(cubemapTex), shaderName(shaderName) {
    SetupSkybox();
}

void Skybox::Render(ShaderManager &shaderManager, const glm::mat4 &view, const glm::mat4 &projection,
                    bool useSkybox) {
    glDepthFunc(GL_LEQUAL);
    shaderManager.Bind(shaderName);

    shaderManager.SetMat4(shaderName, "view", view);
    shaderManager.SetMat4(shaderName, "projection", projection);

    skyboxVAO->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    skyboxVAO->Unbind();

    glDepthFunc(GL_LESS);
}

void Skybox::SetShader(const std::string &newShaderName) {
    shaderName = newShaderName;
}

void Skybox::SetCubemapTexture(const unsigned int newCubemap) {
    cubemapTexture = newCubemap;
}

std::string Skybox::GetShader() const {
    return shaderName;
}

unsigned int Skybox::GetTexture() const {
    return cubemapTexture;
}

GLint Skybox::GetVAO() const {
    return skyboxVAO->GetID();
}