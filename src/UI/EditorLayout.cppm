module; 

#include <glm/glm.hpp>
#include <imgui.h>
#include <entt.hpp>

#include <string>
#include <cstring>
#include <memory>

export module XEngine.UI.EditorLayout;

import XEngine.ECS.ECSWorld;
import XEngine.ECS.Components;

import XEngine.Core.Camera;
import XEngine.Core.Logger;
import XEngine.Core.CommandManager;

import XEngine.Rendering.Renderer;
import XEngine.Rendering.Framebuffer;
import XEngine.Rendering.Light;

import XEngine.Resource.Material.MaterialManager;
import XEngine.Resource.Shader.ShaderManager;

import XEngine.UI.Theme; 

import XEngine.UI.Windows.ViewportWindow;
import XEngine.UI.Windows.MenuBarWindow;
import XEngine.UI.Windows.HierarchyWindow;
import XEngine.UI.Windows.PropertiesWindow;
import XEngine.UI.Windows.InspectorWindow;
import XEngine.UI.Windows.ConsoleWindow;

export class EditorLayout
{
private:
    bool showHierarchy = true;
    bool showInspector = true;
    bool showProperties = true;
    bool showConsole = false;

    std::unique_ptr<Framebuffer> framebuffer;

    ViewportWindow viewportWindow;
    MenuBarWindow menuBarWindow;
    HierarchyWindow hierarchyWindow;
    PropertiesWindow propertiesWindow;
    InspectorWindow inspectorWindow;
    ConsoleWindow consoleWindow;

public:
    EditorLayout()
    {
        framebuffer = std::make_unique<Framebuffer>(1280, 720);
        Logger::Log(LogLevel::INFO, "EditorLayout created with Framebuffer");
    }

    void RenderEditor(ECSWorld* ecs, Camera* camera, Renderer* renderer, 
                     ShaderManager* shaderManager, MaterialManager* materialManager)
    {
        if (!ecs || !camera || !renderer || !materialManager)
        {
            Logger::Log(LogLevel::ERROR, "EditorLayout: nullptr passed to RenderEditor");
            return;
        }

        RenderDockSpace();
        RenderWindows(ecs, camera, renderer, shaderManager, materialManager);
    }
    
    ImVec2 GetViewportSize() const { return viewportWindow.GetViewportSize(); }
    ImVec2 GetViewportPos() const { return viewportWindow.GetViewportPos(); }
    
    bool IsViewportHovered() const { return viewportWindow.IsHovered(); }
    bool IsViewportFocused() const { return viewportWindow.IsFocused(); }

    entt::entity GetSelectedEntity() const { return hierarchyWindow.GetSelectedEntity(); }
    ShaderObj* GetSelectedShader() const { return hierarchyWindow.GetSelectedShader(); }
    std::string GetSelectedMaterial() const { return hierarchyWindow.GetSelectedMaterial(); }

    Framebuffer* GetFramebuffer() const { return framebuffer.get(); }

private:
    void RenderDockSpace()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
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

        menuBarWindow.Render(showHierarchy, showInspector, showProperties, showConsole);

        ImGui::End();
    }

    void RenderWindows(ECSWorld* ecs, Camera* camera, Renderer* renderer,
                      ShaderManager* shaderManager, MaterialManager* materialManager)
    {
        viewportWindow.Render(framebuffer.get());
        
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
            propertiesWindow.Render(camera);
        
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
            
        if (!viewportWindow.IsOpen())
            viewportWindow.SetOpen(true);
    }
};