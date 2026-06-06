#pragma once

#include <string>
#include <memory>
#include <vector>

#include <entt/entt.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/IModule.h"
#include "core/ModuleManager.h"
#include "core/logging/Logger.h"
#include "UI/ImGuiManager.h"
#include "ECS/World.h"
#include "resource/ResourceModule.h"
#include "rendering/RenderingModule.h"
#include "scene/SceneManager.h"

class UIModule : public IModule {
    std::unique_ptr<ImGuiManager> imGuiManager;

    ECSWorld *ecs = nullptr;
    entt::entity *mainCameraEntity = nullptr;
    ModuleManager *mm = nullptr;
    GLFWwindow *window = nullptr;
    SceneManager *m_sceneManager = nullptr;

public:
    UIModule(ECSWorld *ecs, entt::entity *cameraEntity, SceneManager *sceneManager, ModuleManager *mm,
             GLFWwindow *window)
        : ecs(ecs), mainCameraEntity(cameraEntity), m_sceneManager(sceneManager), mm(mm), window(window) {
        if (!ecs)
            Logger::Log(LogLevel::ERROR, "RenderingModule: ecs is null!");
    }

    bool Initialize() override {
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

    void Update(float deltaTime) override {
    }

    void RenderUI() {
        /*editorLayout->RenderEditor(
            ecs, 
            *mainCameraEntity,
            mm->GetModule<RenderingModule>("Rendering")->GetRenderer(), 
            mm->GetModule<ResourceModule>("Resource")->GetShaderManager(), 
            mm->GetModule<ResourceModule>("Resource")->GetMaterialManager(),
            m_sceneManager ? m_sceneManager->IsInPlayMode() : false
        );*/
    }

    void Shutdown() override {
        imGuiManager->Shutdown();

        imGuiManager.reset();
        //editorLayout.reset();
    }

    /// @name IModule interface
    /// @{
    const char *GetName() const override { return "UI"; }
    int GetPriority() const override { return 100; }
    bool IsRequired() const override { return true; }
    /// }@

    //EditorLayout* GetEditorLayout() { return editorLayout.get(); }
    ImGuiManager *GetImGuiManager() { return imGuiManager.get(); }
};