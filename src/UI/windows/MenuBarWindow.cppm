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

        RenderOpenModelDialog();
        RenderSaveSceneDialog();
    }

private:
    /* ===================== STATE ===================== */

    bool showOpenModelDialog = false;
    bool showSaveSceneDialog = false;

    char modelPath[512]{};
    char scenePath[512]{};

    /* ===================== FILE MENU ===================== */

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

        if (ImGui::MenuItem("Load Scene"))
            Execute("onLoadScene");

        ImGui::Separator();

        if (ImGui::MenuItem("Exit", "Alt+F4"))
            Execute("onExit");

        ImGui::EndMenu();
    }

    /* ===================== OPEN MODEL DIALOG ===================== */

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

    /* ===================== SAVE SCENE DIALOG ===================== */

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

    /* ===================== CREATE MENU ===================== */

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

        ImGui::EndMenu();
    }

    /* ===================== VIEW MENU ===================== */

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

    /* ===================== HELPERS ===================== */

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
