#include "UIModule.h"
#include <string>
#include "core/logging/Logger.h"
#include "resource/ResourceModule.h"
#include "rendering/RenderingModule.h"

UIModule::UIModule(ECSWorld *ecs, entt::entity *cameraEntity, SceneManager *sceneManager, ModuleManager *mm,
                   GLFWwindow *window)
    : ecs(ecs), mainCameraEntity(cameraEntity), m_sceneManager(sceneManager), mm(mm), window(window) {
    if (!ecs)
        Logger::Log(LogLevel::ERROR, "RenderingModule: ecs is null!");
}

bool UIModule::Initialize() {
    try {
        imGuiManager = std::make_unique<ImGuiManager>();

        if (!imGuiManager->Initialize(window)) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize ImGuiManager");
            return false;
        }

        Logger::Log(LogLevel::INFO,
                    "Successfully created UI Mdoule");

        isInitialized = true;

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception creating ui module: " + std::string(e.what()));

        isInitialized = true;

        return false;
    }
}

void UIModule::Update(float deltaTime) {
}

void UIModule::RenderUI() {
    /*editorLayout->RenderEditor(
        ecs, 
        *mainCameraEntity,
        mm->GetModule<RenderingModule>("Rendering")->GetRenderer(), 
        mm->GetModule<ResourceModule>("Resource")->GetShaderManager(), 
        mm->GetModule<ResourceModule>("Resource")->GetMaterialManager(),
        m_sceneManager ? m_sceneManager->IsInPlayMode() : false
    );*/
}

void UIModule::Shutdown() {
    imGuiManager->Shutdown();

    imGuiManager.reset();
    //editorLayout.reset();
}

const char *UIModule::GetName() const {
    return "UI";
}

int UIModule::GetPriority() const {
    return 100;
}

bool UIModule::IsRequired() const {
    return true;
}

ImGuiManager *UIModule::GetImGuiManager() {
    return imGuiManager.get();
}