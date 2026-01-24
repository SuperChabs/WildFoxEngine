module;

#include <imgui.h>

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
        
        if (ImGui::MenuItem("Exit"))
            if(CommandManager::HasCommand("onExit")) 
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
};