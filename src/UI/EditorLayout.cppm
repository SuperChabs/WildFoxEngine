module; 

#include <imgui.h>

#include <functional>
#include <memory>
#include <string>
#include <cstring>

// ─────────────────────────────────────────────

export module XEngine.UI.EditorLayout;

// ─────────────────────────────────────────────

import XEngine.Scene.SceneManager;
import XEngine.Core.Camera;
import XEngine.Rendering.Renderer;
import XEngine.Rendering.Framebuffer;
import XEngine.Utils.Logger;

export class EditorLayout
{
public:
    EditorLayout()
        : selectedObject(nullptr)
        , showHierarchy(true)
        , showInspector(true)
        , showProperties(true)
        , showConsole(false)
        , viewportSize({800, 600})
        , viewportPos({0, 0})
        , isViewportHovered(false)
        , isViewportFocused(false)
    {
        framebuffer = std::make_unique<Framebuffer>(1280, 720);
        Logger::Log(LogLevel::INFO,
            "EditorLayout created with Framebuffer");
    }

    void RenderEditor(SceneManager* sceneManager,
                      Camera* camera,
                      Renderer* renderer,
                      std::function<void()> onCreateCube)
    {
        if (!sceneManager || !camera || !renderer)
        {
            Logger::Log(LogLevel::ERROR,
                "EditorLayout: nullptr passed to RenderEditor");
            return;
        }

        RenderControlPanel(onCreateCube);

        if (showHierarchy)  RenderSceneHierarchy(sceneManager);
        if (showInspector)  RenderInspector();
        if (showProperties) RenderProperties(camera, renderer);
        if (showConsole)    RenderConsole();

        RenderViewport();
    }

    ImVec2 GetViewportSize()  const { return viewportSize; }
    ImVec2 GetViewportPos()   const { return viewportPos; }
    bool   IsViewportHovered() const { return isViewportHovered; }
    bool   IsViewportFocused() const { return isViewportFocused; }

    SceneObject* GetSelectedObject() const
    {
        return selectedObject;
    }

    Framebuffer* GetFramebuffer() const
    {
        return framebuffer.get();
    }

    void SetSelectedObject(SceneObject* obj)
    {
        selectedObject = obj;
    }

private:
    // ───── state ─────

    SceneObject* selectedObject;

    bool showHierarchy;
    bool showInspector;
    bool showProperties;
    bool showConsole;

    ImVec2 viewportSize;
    ImVec2 viewportPos;
    bool   isViewportHovered;
    bool   isViewportFocused;

    std::unique_ptr<Framebuffer> framebuffer;

    // ───── UI blocks ─────

    void RenderControlPanel(std::function<void()> onCreateCube)
    {
        ImGui::SetNextWindowPos({10, 10}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 150}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Control Panel");

        ImGui::Text("Scene Controls");
        ImGui::Separator();

        if (ImGui::Button("Create Cube", {-1, 30}))
        {
            if (onCreateCube) onCreateCube();
            Logger::Log(LogLevel::INFO, "Cube created");
        }

        if (ImGui::Button("Create Sphere", {-1, 30}))
            Logger::Log(LogLevel::INFO, "Sphere not implemented");

        if (ImGui::Button("Clear Scene", {-1, 30}))
            Logger::Log(LogLevel::INFO, "Clear not implemented");

        ImGui::Separator();
        ImGui::Checkbox("Show Console", &showConsole);

        ImGui::End();
    }

    void RenderViewport()
    {
        ImGui::SetNextWindowPos({300, 10}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({800, 650}, ImGuiCond_FirstUseEver);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
        ImGui::Begin("Game Viewport");

        isViewportHovered = ImGui::IsWindowHovered();
        isViewportFocused = ImGui::IsWindowFocused();

        ImVec2 size = ImGui::GetContentRegionAvail();

        if (size.x > 0 && size.y > 0 &&
            (size.x != viewportSize.x || size.y != viewportSize.y))
        {
            viewportSize = size;
            framebuffer->Resize((int)size.x, (int)size.y);

            Logger::Log(LogLevel::INFO,
                "Viewport resized to " +
                std::to_string((int)size.x) + "x" +
                std::to_string((int)size.y));
        }

        viewportPos = ImGui::GetCursorScreenPos();

        if (framebuffer)
        {
            ImGui::Image(
                (void*)(intptr_t)framebuffer->GetTextureID(),
                size,
                {0, 1},
                {1, 0});
        }
        else
        {
            ImGui::Text("Framebuffer error");
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void RenderSceneHierarchy(SceneManager* sceneManager)
    {
        ImGui::SetNextWindowPos({10, 170}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 400}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Hierarchy", &showHierarchy);

        ImGui::Text("Objects: %zu",
            sceneManager->GetObjectCount());
        ImGui::Separator();

        uint64_t objectToDelete = 0;

        for (auto& [id, objPtr] : sceneManager->GetObjects())
        {
            if (!objPtr) continue;

            SceneObject* obj = objPtr.get();

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (selectedObject == obj)
                flags |= ImGuiTreeNodeFlags_Selected;

            std::string label =
                obj->name + " [" +
                std::to_string(obj->objectID) + "]";

            ImGui::TreeNodeEx(
                (void*)(intptr_t)id,
                flags,
                label.c_str());

            if (ImGui::IsItemClicked())
                selectedObject = obj;

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete"))
                    objectToDelete = obj->objectID;

                ImGui::EndPopup();
            }
        }

        if (objectToDelete)
            sceneManager->RemoveObject(objectToDelete);

        ImGui::End();
    }

    void RenderInspector()
    {
        ImGui::SetNextWindowPos({1110, 10}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({300, 660}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Inspector", &showInspector);

        if (!selectedObject)
        {
            ImGui::TextDisabled("No object selected");
            ImGui::End();
            return;
        }

        char buffer[256];
        std::strncpy(buffer,
            selectedObject->name.c_str(),
            sizeof(buffer));

        ImGui::Text("Name");
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            selectedObject->name = buffer;

        ImGui::Checkbox("Active",
            &selectedObject->isActive);

        ImGui::End();
    }

    void RenderProperties(Camera* camera,
                          Renderer* renderer)
    {
        ImGui::SetNextWindowPos({10, 580}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 250}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Scene Settings", &showProperties);

        if (ImGui::CollapsingHeader("Camera"))
        {
            float speed = camera->GetMovementSpeed();
            if (ImGui::SliderFloat("Speed",
                                   &speed, 1.f, 20.f))
                camera->SetMovementSpeed(speed);
        }

        if (ImGui::CollapsingHeader("Renderer"))
        {
            auto& s = renderer->GetSettings();
            ImGui::Checkbox("Wireframe", &s.enableWireframe);
            ImGui::Checkbox("Depth Test", &s.enableDepthTest);
            ImGui::ColorEdit3("Clear",
                              &s.clearColor[0]);
        }

        ImGui::End();
    }

    void RenderConsole()
    {
        ImGui::SetNextWindowPos({300, 670}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({800, 120}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Console", &showConsole);
        ImGui::Text("[INFO] Editor ready");
        ImGui::End();
    }
};
