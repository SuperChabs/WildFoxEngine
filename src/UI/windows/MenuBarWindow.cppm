module;

#include <imgui.h>
#include <string>
#include <chrono>

export module WFE.UI.Windows.MenuBarWindow;

import WFE.Core.Logger;
import WFE.Core.CommandManager;

import WFE.UI.Theme;

export class MenuBarWindow
{
public:
    void Render(bool& showHierarchy, bool& showInspector, 
                bool& showProperties, bool& showConsole)
    {
        if (ImGui::BeginMenuBar())
        {
            RenderFileMenu();
            RenderCreateMenu();
            RenderViewMenu(showHierarchy, showInspector, 
                          showProperties, showConsole);
            
            ImGui::EndMenuBar();
        }
    }

private:
    void RenderFileMenu()
    {
        if (!ImGui::BeginMenu("File")) return;
        
        if (ImGui::MenuItem("New Scene", "Ctrl+N"))
            if (CommandManager::HasCommand("onNewScene"))
                CommandManager::ExecuteCommand("onNewScene", {});
        
        ImGui::Separator();
        
        if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            if (CommandManager::HasCommand("onSaveScene"))
                CommandManager::ExecuteCommand("onSaveScene", {});
        
        if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
            if (CommandManager::HasCommand("onSaveScene"))
            {
                std::string filename = "scene_" + GetTimestampString() + ".json";
                CommandManager::ExecuteCommand("onSaveScene", {filename});
            }
        
        ImGui::Separator();
        
        if (ImGui::MenuItem("Load Scene", "Ctrl+O"))
            if (CommandManager::HasCommand("onLoadScene"))
                CommandManager::ExecuteCommand("onLoadScene", {});
        
        if (ImGui::BeginMenu("Recent Scenes"))
        {
            // auto scenes = sceneSerializer->GetAvailableScenes();
            
            if (ImGui::MenuItem("quicksave.json"))
                CommandManager::ExecuteCommand("onLoadScene", {std::string("quicksave.json")});
            
            if (ImGui::MenuItem("scene.json"))
                CommandManager::ExecuteCommand("onLoadScene", {std::string("scene.json")});
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("List All Scenes"))
                if (CommandManager::HasCommand("onListScenes"))
                    CommandManager::ExecuteCommand("onListScenes", {});
            
            ImGui::EndMenu();
        }
        
        ImGui::Separator();
        
        if (ImGui::MenuItem("Exit", "Alt+F4"))
            if (CommandManager::HasCommand("onExit"))
                CommandManager::ExecuteCommand("onExit", {});
        
        ImGui::EndMenu();
    }
    
    void RenderCreateMenu()
    {
        if (!ImGui::BeginMenu("Create")) return;
        
        RenderMeshSubmenu();
        RenderLightSubmenu();
        
        ImGui::EndMenu();
    }
    
    void RenderMeshSubmenu()
    {
        if (!ImGui::BeginMenu("Mesh")) return;
        
        if (ImGui::MenuItem("Cube"))
            if(CommandManager::HasCommand("onCreateCube")) 
                CommandManager::ExecuteCommand("onCreateCube", {});
        
        if (ImGui::MenuItem("Plane"))
            if(CommandManager::HasCommand("onCreatePlane")) 
                CommandManager::ExecuteCommand("onCreatePlane", {});

        if (ImGui::MenuItem("Sphere"))
            Logger::Log(LogLevel::INFO, "Sphere not implemented");
        
        ImGui::EndMenu();
    }
    
    void RenderLightSubmenu()
    {
        if (!ImGui::BeginMenu("Light")) return;
        
        if (ImGui::MenuItem("Directional Light"))
            if(CommandManager::HasCommand("onCreateDirectionalLight")) 
                CommandManager::ExecuteCommand("onCreateDirectionalLight", {});
        
        if (ImGui::MenuItem("Point Light"))
            if(CommandManager::HasCommand("onCreatePointLight")) 
                CommandManager::ExecuteCommand("onCreatePointLight", {});
        
        if (ImGui::MenuItem("Spot Light"))
            if(CommandManager::HasCommand("onCreateSpotLight")) 
                CommandManager::ExecuteCommand("onCreateSpotLight", {});
        
        ImGui::EndMenu();
    }
    
    void RenderViewMenu(bool& showHierarchy, bool& showInspector,
                        bool& showProperties, bool& showConsole)
    {
        if (!ImGui::BeginMenu("View")) return;
        
        ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
        ImGui::MenuItem("Inspector", nullptr, &showInspector);
        ImGui::MenuItem("Properties", nullptr, &showProperties);
        ImGui::MenuItem("Console", nullptr, &showConsole);
        
        ImGui::Separator();
        
        RenderThemeSubmenu();
        
        ImGui::EndMenu();
    }
    
    void RenderThemeSubmenu()
    {
        if (!ImGui::BeginMenu("Theme")) return;
        
        if (ImGui::MenuItem("Dark")) 
            Theme::ApplyTheme(ThemeStyle::Dark);
        if (ImGui::MenuItem("Light")) 
            Theme::ApplyTheme(ThemeStyle::Light);
        
        ImGui::Separator();
        
        if (ImGui::MenuItem("Custom")) 
            Theme::ApplyTheme(ThemeStyle::Custom);
        if (ImGui::MenuItem("Red Accent"))
            Theme::ApplyTheme(ThemeStyle::RedAccent);
        if (ImGui::MenuItem("Enemymouse (Cyan)"))
            Theme::ApplyTheme(ThemeStyle::Enemymouse);
        if (ImGui::MenuItem("Adobe Spectrum"))
            Theme::ApplyTheme(ThemeStyle::AdobeSpectrum);
        if (ImGui::MenuItem("LED Synthmaster (Green)"))   
            Theme::ApplyTheme(ThemeStyle::LedSynthmaster);
        if (ImGui::MenuItem("Dougbinks Light"))          
            Theme::ApplyTheme(ThemeStyle::DougbinksLight);
        if (ImGui::MenuItem("Dougbinks Dark"))             
            Theme::ApplyTheme(ThemeStyle::DougbinksDark);
        
        ImGui::Separator();
        
        if (ImGui::MenuItem("Unreal Engine")) 
            Theme::ApplyTheme(ThemeStyle::UnrealEngine);
        if (ImGui::MenuItem("Unity")) 
            Theme::ApplyTheme(ThemeStyle::Unity);
        if (ImGui::MenuItem("Visual Studio")) 
            Theme::ApplyTheme(ThemeStyle::VisualStudio);
        
        ImGui::EndMenu();
    }

    std::string GetTimestampString()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::tm tm_snapshot;
        localtime_r(&time, &tm_snapshot);
        
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm_snapshot);
        
        return std::string(buffer);
    }
};