#include "rendering/Primitives.h"
#include <glad/glad.h>

PrimitivesRenderer::PrimitivesRenderer()
    : cubeVAO(nullptr), cubeVBO(nullptr),
      quadVAO(nullptr), quadVBO(nullptr),
      planeVAO(nullptr), planeVBO(nullptr),
      initialized(false)
{
}

PrimitivesRenderer::~PrimitivesRenderer()
{
    Cleanup();
}

void PrimitivesRenderer::Initialize()
{
    if (initialized) return;
    
    SetupCube();
    SetupQuad();
    SetupPlane();
    
    initialized = true;
}

void PrimitivesRenderer::Cleanup()
{
    if (!initialized) return;
    
    delete cubeVAO;
    delete cubeVBO;
    delete quadVAO;
    delete quadVBO;
    delete planeVAO;
    delete planeVBO;
    
    cubeVAO = nullptr;
    cubeVBO = nullptr;
    quadVAO = nullptr;
    quadVBO = nullptr;
    planeVAO = nullptr;
    planeVBO = nullptr;
    
    initialized = false;
}

void PrimitivesRenderer::RenderCube()
{
    cubeVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeVAO->Unbind();
}

void PrimitivesRenderer::RenderQuad()
{
    quadVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    quadVAO->Unbind();
}

void PrimitivesRenderer::RenderPlane()
{
    planeVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    planeVAO->Unbind();
}

void PrimitivesRenderer::SetupCube()
{
    float cubeVertices[] = 
    {
        // позиції           // нормалі          // текст. коорд.
        // задняя грань
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
        
        // передняя грань
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        
        // левая грань
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        
        // правая грань
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
        
        // нижняя грань
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        
        // верхняя грань
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
    };
    
    cubeVAO = new VertexArray();
    cubeVBO = new VertexBuffer();
    
    cubeVBO->SetData(cubeVertices, sizeof(cubeVertices));
    cubeVAO->Bind();
    cubeVBO->Bind();
    cubeVAO->AddAttribute(0, 3, GL_FLOAT, false, 8 * sizeof(float), 0);
    cubeVAO->AddAttribute(1, 3, GL_FLOAT, false, 8 * sizeof(float), 3 * sizeof(float));
    cubeVAO->AddAttribute(2, 2, GL_FLOAT, false, 8 * sizeof(float), 6 * sizeof(float));
    cubeVAO->Unbind();
}

void PrimitivesRenderer::SetupQuad()
{
    using namespace glm;
    
    vec3 pos1(-1.0f,  1.0f, 0.0f);
    vec3 pos2(-1.0f, -1.0f, 0.0f);
    vec3 pos3( 1.0f, -1.0f, 0.0f);
    vec3 pos4( 1.0f,  1.0f, 0.0f);
    
    vec2 uv1(0.0f, 1.0f);
    vec2 uv2(0.0f, 0.0f);
    vec2 uv3(1.0f, 0.0f);
    vec2 uv4(1.0f, 1.0f);
    
    vec3 nm(0.0f, 0.0f, 1.0f);
    
    // Вычисление тангентов и битангентов
    vec3 tangent1, bitangent1, tangent2, bitangent2;
    
    // Трикутник 1
    vec3 edge1 = pos2 - pos1;
    vec3 edge2 = pos3 - pos1;
    vec2 deltaUV1 = uv2 - uv1;
    vec2 deltaUV2 = uv3 - uv1;
    
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    
    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    
    // Трикутник 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;
    
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    
    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    
    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    
    float quadVertices[] = {
        // позиції          // нормалі        // текст. коорд. // тангенти                      // бітангенти
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };
    
    quadVAO = new VertexArray();
    quadVBO = new VertexBuffer();
    
    quadVBO->SetData(quadVertices, sizeof(quadVertices));
    quadVAO->Bind();
    quadVBO->Bind();
    quadVAO->AddAttribute(0, 3, GL_FLOAT, false, 14 * sizeof(float), 0);
    quadVAO->AddAttribute(1, 3, GL_FLOAT, false, 14 * sizeof(float), 3 * sizeof(float));
    quadVAO->AddAttribute(2, 2, GL_FLOAT, false, 14 * sizeof(float), 6 * sizeof(float));
    quadVAO->AddAttribute(3, 3, GL_FLOAT, false, 14 * sizeof(float), 8 * sizeof(float));
    quadVAO->AddAttribute(4, 3, GL_FLOAT, false, 14 * sizeof(float), 11 * sizeof(float));
    quadVAO->Unbind();
}

void PrimitivesRenderer::SetupPlane()
{
    float planeVertices[] = 
    {
        // позиції          // нормалі         // текст. коорд.
         50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  20.0f,  0.0f,
        -50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 20.0f,
        
         50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  20.0f,  0.0f,
        -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 20.0f,
         50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  20.0f, 20.0f
    };
    
    planeVAO = new VertexArray();
    planeVBO = new VertexBuffer();
    
    planeVBO->SetData(planeVertices, sizeof(planeVertices));
    planeVAO->Bind();
    planeVBO->Bind();
    planeVAO->AddAttribute(0, 3, GL_FLOAT, false, 8 * sizeof(float), 0);
    planeVAO->AddAttribute(1, 3, GL_FLOAT, false, 8 * sizeof(float), 3 * sizeof(float));
    planeVAO->AddAttribute(2, 2, GL_FLOAT, false, 8 * sizeof(float), 6 * sizeof(float));
    planeVAO->Unbind();
}