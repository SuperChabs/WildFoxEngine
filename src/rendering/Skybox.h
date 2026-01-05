#ifndef SKYBOX_H
#define SKYBOX_H

#include "rendering/Buffer.h"
#include "core/Shader.h"
#include "core/Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Skybox 
{
private:
    VertexArray* skyboxVAO;
    VertexBuffer* skyboxVBO;
    unsigned int cubemapTexture;
    Shader* shader;
    
    void SetupSkybox();

public:
    Skybox(unsigned int cubemapTex, Shader* skyboxShader);
    ~Skybox();
    
    void Render(const Camera& camera, const glm::mat4& projection);
    
    void SetShader(Shader* newShader) { shader = newShader; }
    Shader* GetShader() { return shader; }
    unsigned int GetTexture() const { return cubemapTexture; }
};

#endif