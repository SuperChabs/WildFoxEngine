#include "rendering/Renderer.h"
#include "utils/Logger.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer()
    : primitives(nullptr)
{
}

Renderer::~Renderer()
{
    if (primitives)
        delete primitives;
}

void Renderer::Initialize()
{
    primitives = new PrimitivesRenderer();
    primitives->Initialize();
    
    ApplySettings();

    Logger::Info("Renderer system initialized");
}

void Renderer::ApplySettings()
{
    if (settings.enableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
        
    if (settings.enableCullFace)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
        
    if (settings.enableMultisampling)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);
        
    if (settings.enableWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::Clear()
{
    glClearColor(settings.clearColor.r, settings.clearColor.g, 
                 settings.clearColor.b, settings.clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetClearColor(const glm::vec4& color)
{
    settings.clearColor = color;
}

void Renderer::BeginFrame()
{
    Clear();
}

void Renderer::EndFrame()
{
}

void Renderer::RenderScene(SceneManager& scene, Camera& camera, Shader& shader,
                          int screenWidth, int screenHeight)
{
    shader.use();
    
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.GetZoom()),
        (float)screenWidth / (float)screenHeight,
        0.1f, 100.0f
    );
    glm::mat4 view = camera.GetViewMatrix();
    
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("viewPos", camera.GetPosition());
    
    scene.RenderAll(shader);
}

void Renderer::EnableWireframe(bool enable)
{
    settings.enableWireframe = enable;
    ApplySettings();
}

void Renderer::EnableDepthTest(bool enable)
{
    settings.enableDepthTest = enable;
    ApplySettings();
}

void Renderer::EnableCullFace(bool enable)
{
    settings.enableCullFace = enable;
    ApplySettings();
}