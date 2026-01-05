module;

#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module XEngine.Rendering.Renderer;

import XEngine.Core.Camera;
import XEngine.Core.Shader;
import XEngine.Scene.SceneManager;
import XEngine.Rendering.Primitive.PrimitivesFactory;

export struct RenderSettings 
{
    bool enableDepthTest = true;
    bool enableCullFace = true;
    bool enableWireframe = false;
    bool enableMultisampling = true;
    glm::vec4 clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
};

export class Renderer
{
private:
    RenderSettings settings;
    std::unique_ptr<PrimitivesFactory> primitives;

    void ApplySettings()
    {
        if (settings.enableDepthTest) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);

        if (settings.enableCullFace) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);

        if (settings.enableMultisampling) glEnable(GL_MULTISAMPLE);
        else glDisable(GL_MULTISAMPLE);

        if (settings.enableWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

public:
    Renderer() = default;

    void Initialize()
    {
        ApplySettings();
        // Logger::Log(LogLevel::INFO, "Renderer system initialized"); // якщо хочеш лог
    }

    void Clear()
    {
        glClearColor(settings.clearColor.r, settings.clearColor.g, 
                     settings.clearColor.b, settings.clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SetClearColor(const glm::vec4& color) { settings.clearColor = color; }

    void BeginFrame() { Clear(); }
    void EndFrame() {}

    void RenderScene(SceneManager& scene, Camera& camera, Shader& shader,
                     int screenWidth, int screenHeight)
    {
        shader.use();

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.GetZoom()),
            float(screenWidth) / float(screenHeight),
            0.1f, 100.0f
        );

        glm::mat4 view = camera.GetViewMatrix();

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("viewPos", camera.GetPosition());

        scene.RenderAll(shader);
    }

    RenderSettings& GetSettings() { return settings; }
    PrimitivesFactory* GetPrimitives() { return primitives.get(); }

    void EnableWireframe(bool enable) { settings.enableWireframe = enable; ApplySettings(); }
    void EnableDepthTest(bool enable) { settings.enableDepthTest = enable; ApplySettings(); }
    void EnableCullFace(bool enable) { settings.enableCullFace = enable; ApplySettings(); }
};
