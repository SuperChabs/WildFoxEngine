#ifndef RENDERER_H
#define RENDERER_H

#include "core/Camera.h"
#include "core/Shader.h"
#include "rendering/Primitives.h"
#include "scene/SceneManager.h"
#include <glm/glm.hpp>

struct RenderSettings 
{
    bool enableDepthTest = true;
    bool enableCullFace = true;
    bool enableWireframe = false;
    bool enableMultisampling = true;
    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
};

class Renderer 
{
private:
    RenderSettings settings;
    PrimitivesRenderer* primitives;
    
    void ApplySettings();

public:
    Renderer();
    ~Renderer();
    
    void Initialize();
    void Clear();
    void SetClearColor(const glm::vec4& color);
    
    void BeginFrame();
    void EndFrame();
    
    void RenderScene(SceneManager& scene, Camera& camera, Shader& shader, 
                     int screenWidth, int screenHeight);
    
    RenderSettings& GetSettings() { return settings; }
    PrimitivesRenderer* GetPrimitives() { return primitives; }
    
    void EnableWireframe(bool enable);
    void EnableDepthTest(bool enable);
    void EnableCullFace(bool enable);
};

#endif