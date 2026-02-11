module; 

#include <glm/glm.hpp>
#include <imgui.h>
#include <entt/entt.hpp>

#include <string>
#include <cstring>
#include <memory>

export module WFE.UI.EditorLayout;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

import WFE.Core.Logger;
import WFE.Core.CommandManager;

import WFE.Rendering.Renderer;
import WFE.Rendering.Core.Framebuffer;
import WFE.Scene.Light;

import WFE.Resource.Material.MaterialManager;
import WFE.Resource.Shader.ShaderManager;

import WFE.UI.Theme; 

import WFE.UI.Windows;

export class EditorLayout
{
private:
    bool showHierarchy = true;
    bool showInspector = true;
    bool showProperties = true;
    bool showConsole = false;

    std::unique_ptr<Framebuffer> sceneFramebuffer;
    std::unique_ptr<Framebuffer> gameFramebuffer;

    ViewportWindow sceneViewportWindow;
    ViewportWindow gameViewportWindow;

    MenuBarWindow menuBarWindow;
    HierarchyWindow hierarchyWindow;
    PropertiesWindow propertiesWindow;
    InspectorWindow inspectorWindow;
    ConsoleWindow consoleWindow;

public:
    EditorLayout()
    {
        sceneFramebuffer = std::make_unique<Framebuffer>(1280, 720);
        gameFramebuffer = std::make_unique<Framebuffer>(1280, 720);
        Logger::Log(LogLevel::INFO, "EditorLayout created with Framebuffer");
    }

    void RenderEditor(ECSWorld* ecs, entt::entity cameraEntity, Renderer* renderer, 
                     ShaderManager* shaderManager, MaterialManager* materialManager, bool isPlayMode)
    {
        if (!ecs || cameraEntity == entt::null || !renderer || !materialManager)
        {
            Logger::Log(LogLevel::ERROR, "EditorLayout: nullptr passed to RenderEditor");
            return;
        }

        RenderDockSpace(isPlayMode);
        RenderWindows(ecs, cameraEntity, renderer, shaderManager, materialManager, isPlayMode);
    }
    
    ImVec2 GetSceneViewportSize() const { return sceneViewportWindow.GetViewportSize(); }
    ImVec2 GetSceneViewportPos() const { return sceneViewportWindow.GetViewportPos(); }
    bool IsSceneViewportHovered() const { return sceneViewportWindow.IsHovered(); }
    bool IsSceneViewportFocused() const { return sceneViewportWindow.IsFocused(); }

    ImVec2 GetGameViewportSize() const { return gameViewportWindow.GetViewportSize(); }
    ImVec2 GetGameViewportPos() const { return gameViewportWindow.GetViewportPos(); }
    bool IsGameViewportHovered() const { return gameViewportWindow.IsHovered(); }
    bool IsGameViewportFocused() const { return gameViewportWindow.IsFocused(); }

    entt::entity GetSelectedEntity() const { return hierarchyWindow.GetSelectedEntity(); }
    ShaderObj* GetSelectedShader() const { return hierarchyWindow.GetSelectedShader(); }
    std::string GetSelectedMaterial() const { return hierarchyWindow.GetSelectedMaterial(); }

    Framebuffer* GetSceneFramebuffer() const { return sceneFramebuffer.get(); }
    Framebuffer* GetGameFramebuffer() const { return gameFramebuffer.get(); }

private:
    void RenderDockSpace(bool isPlayMode)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        if (!viewport)
        {
            Logger::Log(LogLevel::ERROR, "Main viewport is NULL!");
            return;
        }
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        menuBarWindow.Render(showHierarchy, showInspector, showProperties, showConsole, isPlayMode);

        ImGui::End();
    }

    void RenderWindows(ECSWorld* ecs, entt::entity cameraEntity, Renderer* renderer,
                      ShaderManager* shaderManager, MaterialManager* materialManager,
                      bool isPlayMode)
    {
        entt::entity actualCamera = cameraEntity;

        if (ecs && (!ecs->IsValid(actualCamera) || !ecs->HasComponent<CameraComponent>(actualCamera)))
            {
                actualCamera = entt::null;
                auto view = ecs->GetRegistry().view<CameraComponent>();
                if (!view.empty()) 
                    actualCamera = view.front();
            }

        sceneViewportWindow.Render("Scene", sceneFramebuffer.get(), ecs, actualCamera, 
                                    GetSelectedEntity(), isPlayMode);
        gameViewportWindow.Render("Game",gameFramebuffer.get(), ecs, actualCamera, 
                                    entt::null, isPlayMode);
        
        if (showHierarchy)
            hierarchyWindow.Render(ecs, shaderManager, materialManager);
        
        if (showInspector)
            inspectorWindow.Render(
                ecs, 
                hierarchyWindow.GetSelectedEntity(), 
                materialManager,
                shaderManager,
                hierarchyWindow.GetSelectedShader()
            );
        
        if (showProperties)
            propertiesWindow.Render(ecs, cameraEntity, renderer);
        
        if (showConsole)
            consoleWindow.Render();
        
        SyncWindowFlags();
    }
    
    void SyncWindowFlags()
    {
        if (!hierarchyWindow.IsOpen())
            showHierarchy = false;
            
        if (!inspectorWindow.IsOpen())
            showInspector = false;
            
        if (!propertiesWindow.IsOpen())
            showProperties = false;
            
        if (!consoleWindow.IsOpen())
            showConsole = false;
            
        if (!sceneViewportWindow.IsOpen())
            sceneViewportWindow.SetOpen(true);

        if (!gameViewportWindow.IsOpen())
            gameViewportWindow.SetOpen(true);
    }
};