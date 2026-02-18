module;

#include <memory>

#include <entt/entt.hpp>
#include <GLFW/glfw3.h>

export module WFE.UI.UIModule;

import WFE.Core.IModule;
import WFE.Core.ModuleManager;
import WFE.Core.Logger;
import WFE.UI.EditorLayout;
import WFE.UI.ImGuiManager;
import WFE.ECS.ECSWorld;
import WFE.Resource.ResourceModule;
import WFE.Rendering.RenderingModule;
import WFE.Scene.SceneManager;

export class UIModule : public IModule
{
    std::unique_ptr<EditorLayout> editorLayout;
    std::unique_ptr<ImGuiManager> imGuiManager;

    ECSWorld* ecs = nullptr;
    entt::entity* mainCameraEntity = nullptr;
    ModuleManager* mm = nullptr;
    GLFWwindow* window = nullptr;
    SceneManager* m_sceneManager = nullptr;

public:
    UIModule(ECSWorld* ecs, entt::entity* cameraEntity, SceneManager* sceneManager, ModuleManager* mm, GLFWwindow* window)
        : ecs(ecs), mainCameraEntity(cameraEntity), m_sceneManager(sceneManager), mm(mm), window(window)
    {
        if (!ecs)
            Logger::Log(LogLevel::ERROR, "RenderingModule: ecs is null!");
    }

    bool Initialize() override
    {
        try 
        {
            imGuiManager = std::make_unique<ImGuiManager>();
            editorLayout = std::make_unique<EditorLayout>();
            
            if (!imGuiManager->Initialize(window))
            {
                Logger::Log(LogLevel::ERROR, "Failed to initialize ImGuiManager");
                return false;
            }

            Logger::Log(LogLevel::INFO, 
                "Successfully created UI Mdoule");

            isInitialized = true;

            return true;
        } 
        catch (const std::exception& e) 
        {
            Logger::Log(LogLevel::ERROR, 
                "Exception creating ui module: " + std::string(e.what()));

            isInitialized = true;

            return false;
        }
    }

    void Update(float deltaTime) override
    {}

    void RenderUI()
    {
        editorLayout->RenderEditor(
            ecs, 
            *mainCameraEntity,
            mm->GetModule<RenderingModule>("Rendering")->GetRenderer(), 
            mm->GetModule<ResourceModule>("Resource")->GetShaderManager(), 
            mm->GetModule<ResourceModule>("Resource")->GetMaterialManager(),
            m_sceneManager ? m_sceneManager->IsInPlayMode() : false
        );
    }

    void Shutdown() override 
    {
        imGuiManager->Shutdown();

        imGuiManager.reset();
        editorLayout.reset();

    }

    /// @name IModule interface
    /// @{
    const char* GetName() const override{ return "UI"; }
    int GetPriority() const override { return 100; }
    bool IsRequired() const override { return true; }
    /// }@

    EditorLayout* GetEditorLayout() { return editorLayout.get(); }
    ImGuiManager* GetImGuiManager() { return imGuiManager.get(); }
};