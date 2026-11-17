#include "UIModule.h"

#include <string>

#include "core/logging/Logger.h"
#include "resource/ResourceModule.h"
#include "rendering/RenderingModule.h"
#include "scene/SceneModule.h"

UIModule::UIModule(ECSWorld *ecs, SceneManager *sceneManager, ModuleManager *mm,
                   GLFWwindow *window)
    : ecs(ecs), mm(mm), window(window), m_sceneManager(sceneManager) {
    if (!ecs)
        Logger::Log(LogLevel::ERROR, "RenderingModule: ecs is null!");
}

bool UIModule::OnInitialize() {
    try {
        imGuiManager = std::make_unique<ImGuiManager>();
        if (!imGuiManager->Initialize(window)) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize ImGuiManager");
            return false;
        }

        debugOverlay = std::make_unique<DebugOverlay>();

        Logger::Log(LogLevel::INFO,
                    "Successfully created UI Module");

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception creating ui module: " + std::string(e.what()));

        return false;
    }
}

void UIModule::Update(float deltaTime) {
}

void UIModule::RenderUI(EditorCamera &editorCamera, PhysicsDebugRenderSystem *physicsDebugRenderSystem) {
    debugOverlay->Render(ecs, mm->GetModule<ResourceModule>()->GetMaterialManager(),
                         mm->GetModule<SceneModule>()->GetSceneSerializer(), editorCamera,
                         mm->GetModule<PhysicsModule>(), physicsDebugRenderSystem);
}

void UIModule::Shutdown() {
    imGuiManager->Shutdown();

    imGuiManager.reset();
    debugOverlay.reset();
}

const char *UIModule::GetName() const {
    return "UI";
}

bool UIModule::IsRequired() const {
    return true;
}

DebugOverlay *UIModule::GetDebugOverlay() {
    return debugOverlay.get();
}

ImGuiManager *UIModule::GetImGuiManager() {
    return imGuiManager.get();
}