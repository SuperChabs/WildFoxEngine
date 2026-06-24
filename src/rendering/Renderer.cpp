#include "Renderer.h"
#include <string>
#include <vector>
#include "rendering/pipeline/PipelineBuilder.h"
#include "rendering/pipeline/ForwardPipeline.h"
#include "core/logging/Logger.h"
#include "core/CommandManager.h"

Renderer::Renderer(ShaderManager *sm, ECSWorld *w, TextureManager *tm)
    : shaderManager(sm)
      , world(w) {
    if (!shaderManager)
        Logger::Log(LogLevel::ERROR, "Renderer: shaderManager is null!");
    if (!world)
        Logger::Log(LogLevel::ERROR, "Renderer: world is null!");

    context = std::make_unique<GLContext>();
    renderSystem = std::make_unique<RenderSystem>();
    lightSystem = std::make_unique<LightSystem>();
    m_icon = std::make_unique<IconRenderSystem>(tm);

    RegisterRenderCommands();
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(GLuint skyboxVAO, GLuint cubemapTexture) {
    if (initialized) return true;

    ApplySettings();

    PipelineBuilder builder;
    pipeline = builder
            .SetContext(context.get())
            .SetShaderManager(shaderManager)
            .SetWorld(world)
            .SetSkybox(skyboxVAO, cubemapTexture)
            .SetType(PipelineType::FORWARD)
            .Build();

    if (!pipeline) {
        Logger::Log(LogLevel::ERROR, "Failed to build render pipeline");
        return false;
    }

    lastFPSUpdate = Clock::now();
    initialized = true;
    Logger::Log(LogLevel::INFO, "Renderer initialized successfully");
    return true;
}

void Renderer::BeginFrame() {
    if (!initialized) return;

    frameStart = Clock::now();
    context->ResetStats();
    stats.Reset();

    context->ClearColor(
        config.clearColor.x, config.clearColor.y,
        config.clearColor.z, config.clearColor.w);
    context->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Render(ECSWorld &ecs, entt::entity cameraEntity,
                      int width, int height) {
    if (!initialized || !pipeline) return;
    pipeline->Execute(ecs, cameraEntity, width, height);
}

void Renderer::EndFrame() {
    if (!initialized) return;

    auto now = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - frameStart);
    stats.frameTime = duration.count() / 1000.0f;

    frameCount++;
    fpsFrameCount++;

    auto fpsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFPSUpdate);
    if (fpsDuration.count() >= 1000) {
        stats.fps = fpsFrameCount / (fpsDuration.count() / 1000.0f);
        fpsFrameCount = 0;
        lastFPSUpdate = now;
    }

    const auto &cs = context->GetStats();
    stats.drawCalls = cs.drawCalls;
    stats.stateChanges = cs.stateChanges;
    stats.vertexCount = cs.vertexCount;
    stats.triangleCount = cs.triangleCount;

    if (frameCount % 60 == 0) LogStats();
}

void Renderer::Shutdown() {
    if (!initialized) return;
    pipeline.reset();
    renderSystem.reset();
    lightSystem.reset();
    m_icon.reset();
    context.reset();
    initialized = false;
    Logger::Log(LogLevel::INFO, "Renderer shutdown");
}

void Renderer::SetClearColor(const glm::vec4 &color) {
    config.clearColor = color;
}

void Renderer::SetWireframe(bool enable) {
    config.enableWireframe = enable;
    glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
}

void Renderer::SetEnableShadows(bool enable) {
    config.enableShadows = enable;

    if (!pipeline) return;

    if (auto *fp = dynamic_cast<ForwardPipeline *>(pipeline.get())) {
        if (auto *sp = fp->GetShadowPass())
            sp->SetEnabled(enable);
    }

    Logger::Log(LogLevel::INFO,
                std::string("Shadows ") + (enable ? "enabled" : "disabled"));
}

GLContext *Renderer::GetContext() {
    return context.get();
}

RenderPipeline *Renderer::GetPipeline() {
    return pipeline.get();
}

const RenderStats &Renderer::GetStats() const {
    return stats;
}

RendererConfig &Renderer::GetConfig() {
    return config;
}

ECSWorld *Renderer::GetWorld() {
    return world;
}

ShaderManager *Renderer::GetShaderManager() {
    return shaderManager;
}

IconRenderSystem *Renderer::GetIcon() {
    return m_icon.get();
}

bool Renderer::IsInitialized() const {
    return initialized;
}

void Renderer::ApplySettings() {
    if (config.enableDepthTest) context->SetDepthTest(true);
    if (config.enableCullFace) context->SetCullFace(true);
    if (config.enableMultisampling) glEnable(GL_MULTISAMPLE);
    if (config.enableWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Renderer::LogStats() const {
    Logger::Log(LogLevel::DEBUG,
                "FPS: " + std::to_string(static_cast<int>(stats.fps)) +
                " | Frame: " + std::to_string(stats.frameTime) + "ms" +
                " | Draws: " + std::to_string(stats.drawCalls) +
                " | Tris: " + std::to_string(stats.triangleCount));
}

void Renderer::RegisterRenderCommands() {
    if (CommandManager::HasCommand("Renderer_RenderGeometry")) {
        Logger::Log(LogLevel::WARNING,
                    "Renderer: Renderer_RenderGeometry already registered — skipping (hot-reload path?)");
    } else
        CommandManager::RegisterCommand("Renderer_RenderGeometry",
                                        [this](const CommandArgs &args) {
                                            if (args.size() < 3) {
                                                Logger::Log(LogLevel::ERROR,
                                                            "Renderer_RenderGeometry: needs ≥3 args");
                                                return;
                                            }

                                            const auto &view = std::get<glm::mat4>(args[0]);
                                            const auto &projection = std::get<glm::mat4>(args[1]);
                                            const auto &shaderName = std::get<std::string>(args[2]);

                                            shaderManager->Bind(shaderName);
                                            shaderManager->SetMat4(shaderName, "projection", projection);
                                            shaderManager->SetMat4(shaderName, "view", view);

                                            bool shadowsEnabled = false;
                                            if (args.size() >= 5) {
                                                const auto &lightSpaceMatrices = std::get<std::vector<glm::mat4> >(
                                                    args[3]);
                                                int shadowTexID = std::get<int>(args[4]);

                                                for (int i = 0; i < lightSpaceMatrices.size(); i++)
                                                    shaderManager->SetMat4(shaderName,
                                                                           "lightSpaceMatrices[" + std::to_string(i)
                                                                           + "]", lightSpaceMatrices[i]);

                                                if (shadowTexID != 0) {
                                                    shadowsEnabled = true;
                                                    shaderManager->SetBool(shaderName, "shadowsEnabled", true);
                                                    shaderManager->SetInt(
                                                        shaderName, "shadowMapArray", SHADOW_MAP_SLOT);

                                                    glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_SLOT);
                                                    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowTexID);
                                                }
                                            }

                                            if (!shadowsEnabled)
                                                shaderManager->SetBool(shaderName, "shadowsEnabled", false);

                                            const std::vector<int> *shadowMapIndices = nullptr;
                                            if (args.size() >= 6) {
                                                shadowMapIndices = &std::get<std::vector<int> >(args[5]);
                                            }

                                            lightSystem->Update(*world, *shaderManager, shaderName,
                                                                shadowMapIndices);
                                            renderSystem->Update(*world, *shaderManager, shaderName);

                                            shaderManager->Unbind();
                                        });

    if (!CommandManager::HasCommand("Renderer_RenderUI"))
        CommandManager::RegisterCommand("Renderer_RenderUI",
                                        [this](const CommandArgs &args) {
                                            if (args.size() < 3) return;

                                            const auto &view = std::get<glm::mat4>(args[0]);
                                            const auto &projection = std::get<glm::mat4>(args[1]);
                                            const auto &shaderName = std::get<std::string>(args[2]);

                                            shaderManager->Bind(shaderName);
                                            shaderManager->SetMat4(shaderName, "projection", projection);
                                            shaderManager->SetMat4(shaderName, "view", view);
                                            shaderManager->Unbind();
                                        });

    Logger::Log(LogLevel::INFO, "Render commands registered");
}