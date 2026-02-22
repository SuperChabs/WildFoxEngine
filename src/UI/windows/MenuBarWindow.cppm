module;

#include <imgui.h>
#include <string>
#include <cstring>
#include <vector>
#include <variant>

export module WFE.UI.Windows.MenuBarWindow;

import WFE.Core.Logger;
import WFE.Core.CommandManager;
import WFE.UI.Theme;

export class MenuBarWindow
{
public:
    void Render(bool& showHierarchy, bool& showInspector,
                bool& showProperties, bool& showConsole, bool isPlayMode)
    {
        if (ImGui::BeginMenuBar())
        {
            RenderFileMenu();
            RenderCreateMenu();
            RenderViewMenu(showHierarchy, showInspector,
                           showProperties, showConsole);

            ImGui::Separator();
            RenderPlayControls(isPlayMode);

            ImGui::EndMenuBar();
        }

        RenderOpenModelDialog();
        RenderSaveSceneDialog();
        RenderOpenSceneDialog();
    }

private:
    bool showOpenModelDialog = false;
    bool showSaveSceneDialog = false;
    bool showOpenSceneDialog = false;

    char modelPath[512]{};
    char scenePath[512]{};

    void RenderPlayControls(bool isPlayMode)
    {
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (isPlayMode)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
            
            if (ImGui::Button("Stop"))
                Execute("onStopGame");
                
            ImGui::PopStyleColor(3);
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.9f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.7f, 0.1f, 1.0f));
            
            if (ImGui::Button("Play"))
                Execute("onPlayGame");
                
            ImGui::PopStyleColor(3);
        }
        
        ImGui::Spacing();
    }

    void RenderFileMenu()
    {
        if (!ImGui::BeginMenu("File")) return;

        if (ImGui::MenuItem("Open Model...", "Ctrl+O"))
        {
            showOpenModelDialog = true;
            modelPath[0] = '\0';
        }

        if (ImGui::MenuItem("New Scene", "Ctrl+N"))
            Execute("onNewScene");

        ImGui::Separator();

        if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
            Execute("onSaveScene");

        if (ImGui::MenuItem("Save Scene As..."))
        {
            showSaveSceneDialog = true;
            std::strcpy(scenePath, "scene.json");
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Open Scene"))
        {
            showOpenSceneDialog = true;
            std::strcpy(scenePath, "scene.json");
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit", "Alt+F4"))
            Execute("onExit");

        ImGui::EndMenu();
    }

    void RenderOpenModelDialog()
    {
        if (!showOpenModelDialog) return;

        ImGui::OpenPopup("Open Model");

        if (ImGui::BeginPopupModal(
                "Open Model",
                &showOpenModelDialog,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Path to model file:");
            ImGui::InputTextWithHint(
                "##modelpath",
                "/home/user/model.obj",
                modelPath,
                sizeof(modelPath));

            ImGui::Spacing();

            if (ImGui::Button("Open"))
            {
                std::string path = modelPath;

                if (!path.empty())
                {
                    Logger::Log(LogLevel::INFO,
                        "Opening model: " + path);
                    Execute("onLoadModel", path);
                }

                ClosePopup(showOpenModelDialog);
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
                ClosePopup(showOpenModelDialog);

            ImGui::EndPopup();
        }
    }

    void RenderSaveSceneDialog()
    {
        if (!showSaveSceneDialog) return;

        ImGui::OpenPopup("Save Scene");

        if (ImGui::BeginPopupModal(
                "Save Scene",
                &showSaveSceneDialog,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Scene filename:");
            ImGui::InputText("##scenepath",
                             scenePath,
                             sizeof(scenePath));

            ImGui::Spacing();

            if (ImGui::Button("Save"))
            {
                std::string path = scenePath;
                if (!path.empty())
                    Execute("onSaveScene", path);

                ClosePopup(showSaveSceneDialog);
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
                ClosePopup(showSaveSceneDialog);

            ImGui::EndPopup();
        }
    }

    void RenderOpenSceneDialog()
    {
        if (!showOpenSceneDialog) return;

        ImGui::OpenPopup("Open Scene");

        if (ImGui::BeginPopupModal(
                "Open Scene",
                &showOpenSceneDialog,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Scene path:");
            ImGui::InputText("##scenepath",
                             scenePath,
                             sizeof(scenePath));

            ImGui::Spacing();

            if (ImGui::Button("Open"))
            {
                std::string path = scenePath;
                if (path.empty())
                    Execute("onLoadScene", path);

                ClosePopup(showOpenSceneDialog);
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
                ClosePopup(showOpenSceneDialog);

            ImGui::EndPopup();
        }
    }

    void RenderCreateMenu()
    {
        if (!ImGui::BeginMenu("Create")) return;

        if (ImGui::BeginMenu("Mesh"))
        {
            ExecuteItem("Cube", "onCreateCube");
            ExecuteItem("Plane", "onCreatePlane");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light"))
        {
            ExecuteItem("Directional", "onCreateDirectionalLight");
            ExecuteItem("Point", "onCreatePointLight");
            ExecuteItem("Spot", "onCreateSpotLight");
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Camera"))
            Execute("onCreateCamera");

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

        if (ImGui::BeginMenu("Theme"))
        {
            ThemeItem("Dark", ThemeStyle::Dark);
            ThemeItem("Light", ThemeStyle::Light);
            ThemeItem("Unreal Engine", ThemeStyle::UnrealEngine);
            ThemeItem("Unity", ThemeStyle::Unity);
            ThemeItem("Visual Studio", ThemeStyle::VisualStudio);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    void Execute(const char* name)
    {
        CommandArgs args;
        Execute(name, args);
    }

    void Execute(const char* name, const std::string& arg)
    {
        CommandArgs args;
        args.emplace_back(arg);
        Execute(name, args);
    }

    void Execute(const char* name, const CommandArgs& args)
    {
        if (CommandManager::HasCommand(name))
            CommandManager::ExecuteCommand(name, args);
    }

    void ExecuteItem(const char* label, const char* command)
    {
        if (ImGui::MenuItem(label))
            Execute(command);
    }

    void ThemeItem(const char* label, ThemeStyle style)
    {
        if (ImGui::MenuItem(label))
            Theme::ApplyTheme(style);
    }

    void ClosePopup(bool& flag)
    {
        flag = false;
        ImGui::CloseCurrentPopup();
    }
};
