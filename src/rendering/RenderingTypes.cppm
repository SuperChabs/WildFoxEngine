module;

#include <glm/glm.hpp>

export module WFE.Rendering.RenderingTypes;

export struct RendererConfig
{
    bool enableDepthTest = true;
    bool enableCullFace = true;
    bool enableMultisampling = true;
    bool enableWireframe = false;
    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    
    int shadowMapSize = 2048;
    bool enableShadows = false;
};

export struct RenderStats
{
    int drawCalls = 0;
    int stateChanges = 0;
    int vertexCount = 0;
    int triangleCount = 0;
    float frameTime = 0.0f;
    float fps = 0.0f;
    
    void Reset()
    {
        drawCalls = 0;
        stateChanges = 0;
        vertexCount = 0;
        triangleCount = 0;
    }
};
