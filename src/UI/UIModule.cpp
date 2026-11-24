#include "UIModule.h"

#include <string>

#include "core/CoreModule.h"
#include "core/logging/Logger.h"
#include "resource/ResourceModule.h"
#include "rendering/RenderingModule.h"
#include "scene/SceneModule.h"

UIModule::UIModule(ECSWorld *ecs, SceneManager *sceneManager, ModuleManager *mm, GLFWwindow *window)
    : ecs(ecs), mm(mm), window(window), m_sceneManager(sceneManager) {
    if (!ecs) Logger::Log(LogLevel::ERROR, "UIModule: ecs is null!");
}

bool UIModule::OnInitialize() {
    try {
        m_imGuiManager = std::make_unique<ImGuiManager>();
        if (!m_imGuiManager->Initialize(window)) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize ImGuiManager");
            return false;
        }

        m_debugOverlay          = std::make_unique<DebugOverlay>();
        m_physicsDebugSystem    = std::make_unique<PhysicsDebugRenderSystem>();
        m_editorCamera          = std::make_unique<EditorCamera>();

        auto &input        = *mm->GetModule<CoreModule>()->GetInput();
        m_cameraController = std::make_unique<UICameraController>(input, *m_editorCamera);

        Logger::Log(LogLevel::INFO, "Successfully created UI Module");

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR, "Exception creating ui module: " + std::string(e.what()));
        return false;
    }
}

void UIModule::Update(float deltaTime) {
    const auto &renderer        = mm->GetModule<RenderingModule>()->GetRenderer();
    const auto &shaderManager   = renderer->GetShaderManager();
    const auto &matManager      = mm->GetModule<ResourceModule>()->GetMaterialManager();
    const auto &serializer      = mm->GetModule<SceneModule>()->GetSceneSerializer();
    const auto &physicsModule   = mm->GetModule<PhysicsModule>();

    Framebuffer *sceneFB = m_debugOverlay->GetEditorFramebuffer();
    const ImVec2 sceneViewportSize = m_debugOverlay->GetGraphicsViewportSize();
    if (m_debugOverlay->GetEditorViewportWindow()->IsOpen()) {
        sceneFB->Bind();

        renderer->BeginFrame();
        renderer->Render(
            m_editorCamera->camera,
            m_editorCamera->transform,
            m_editorCamera->orientation,
            static_cast<int>(sceneViewportSize.x),
            static_cast<int>(sceneViewportSize.y)
        );
        renderer->EndFrame();

        const glm::mat4 view = m_editorCamera->orientation.GetViewMatrix(m_editorCamera->transform.position);
        const glm::mat4 projection = m_editorCamera->camera.GetProjectionMatrix(
            sceneViewportSize.x / sceneViewportSize.y
        );

        m_physicsDebugSystem->Update(
            *ecs,
            *shaderManager,
            "aabbDebug",
            view,
            projection
        );

        renderer->GetIcon()->Update(
            *ecs,
            *shaderManager,
            "icon",
            m_editorCamera->orientation.GetViewMatrix(m_editorCamera->transform.position)
        );

        sceneFB->Unbind();
    }

    m_cameraController->Update(m_debugOverlay->IsGraphicsViewportHovered());

    m_imGuiManager->BeginFrame();
    m_debugOverlay->Render(ecs, matManager, serializer, *m_editorCamera, physicsModule, m_physicsDebugSystem.get());
    m_imGuiManager->EndFrame();
}

void UIModule::Shutdown() {
    m_imGuiManager->Shutdown();

    m_imGuiManager.reset();
    m_debugOverlay.reset();
}

void UIModule::MouseCallback(GLFWwindow */*window*/, const double xPos, const double yPos) const {
    // camera movement itself is handled by UICameraController
    mm->GetModule<CoreModule>()->GetInput()->UpdateMousePosition(xPos, yPos);
}

const char *UIModule::GetName() const {
    return "UI";
}

bool UIModule::IsRequired() const {
    return true;
}