#ifndef RENDERER_H
#define RENDERER_H

#include "core/Camera.h"
#include "core/Shader.h"
#include "scene/SceneManager.h"
#include "rendering/primitive/PrimitivesFactory.h"

#include <glm/glm.hpp>

#include <memory>

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

    std::unique_ptr<PrimitivesFactory> primitives;
    
    void ApplySettings();

public:
    Renderer();
    
    void Initialize();
    void Clear();
    void SetClearColor(const glm::vec4& color);
    
    void BeginFrame();
    void EndFrame();
    
    void RenderScene(SceneManager& scene, Camera& camera, Shader& shader, 
                     int screenWidth, int screenHeight);
    
    RenderSettings& GetSettings() { return settings; }
    PrimitivesFactory* GetPrimitives() { return primitives.get(); }
    
    void EnableWireframe(bool enable);
    void EnableDepthTest(bool enable);
    void EnableCullFace(bool enable);
};

#endif // RENDERER_H