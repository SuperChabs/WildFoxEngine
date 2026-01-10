module; 

#include <glm/glm.hpp>

#include <imgui.h>

#include <string>
#include <cstring>
#include <memory>

export module XEngine.UI.EditorLayout;

import XEngine.Scene.SceneManager;
import XEngine.Core.Camera;
import XEngine.Core.Logger;
import XEngine.Core.CommandManager;
import XEngine.Rendering.Renderer;
import XEngine.Rendering.Framebuffer;
import XEngine.UI.Theme; 

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

    void RenderEditor(SceneManager* sceneManager, Camera* camera, Renderer* renderer)
    {
        if (!sceneManager || !camera || !renderer)
        {
            Logger::Log(LogLevel::ERROR, "EditorLayout: nullptr passed to RenderEditor");
            return;
        }
        
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

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    if(CommandManager::HasCommand("onExit")) 
                        CommandManager::ExecuteCommand("onExit", {});
                    
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Create"))
            {
                if (ImGui::MenuItem("Cube"))
                    if(CommandManager::HasCommand("onCreateCube")) 
                        CommandManager::ExecuteCommand("onCreateCube", {});
                
                if (ImGui::MenuItem("Sphere"))
                    Logger::Log(LogLevel::INFO, "Sphere not implemented");
                
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
                ImGui::MenuItem("Inspector", nullptr, &showInspector);
                ImGui::MenuItem("Properties", nullptr, &showProperties);
                ImGui::MenuItem("Console", nullptr, &showConsole);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
                ImGui::MenuItem("Inspector", nullptr, &showInspector);
                ImGui::MenuItem("Properties", nullptr, &showProperties);
                ImGui::MenuItem("Console", nullptr, &showConsole);
                
                ImGui::Separator();
                
                if (ImGui::BeginMenu("Theme"))
                {
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
                
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();

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

    SceneObject* GetSelectedObject() const { return selectedObject;}
    Framebuffer* GetFramebuffer() const { return framebuffer.get(); }
    void SetSelectedObject(SceneObject* obj) { selectedObject = obj; }

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

    std::vector<std::function<void()>> editorFunctions;

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

            std::string label = obj->name + " [" + std::to_string(obj->objectID) + "]";

            ImGui::TreeNodeEx((void*)(intptr_t)id, flags, "%s", label.c_str());

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
        ImGui::SetNextWindowPos(ImVec2(1110, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 660), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Inspector", &showInspector);
        
        if (selectedObject)
        {
            ImGui::Text("Object");
            ImGui::Separator();
            
            char buffer[256];
            strncpy(buffer, selectedObject->name.c_str(), sizeof(buffer));
            buffer[255] = '\0';
            
            ImGui::Text("Name:");
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
                selectedObject->name = std::string(buffer);
            ImGui::PopItemWidth();
            
            ImGui::Spacing();
            ImGui::Checkbox("Active", &selectedObject->isActive);
            
            ImGui::Spacing();
            ImGui::Separator();
            
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                glm::vec3 pos = selectedObject->transform.GetPosition();
                ImGui::Text("Position");
                if (ImGui::DragFloat3("##Pos", &pos[0], 0.1f))
                    selectedObject->transform.SetPosition(pos);
                
                ImGui::Spacing();
                ImGui::Separator();
                
                glm::vec3 rot = selectedObject->transform.GetRotation();
                ImGui::Text("Rotation");
                if (ImGui::DragFloat3("##Rot", &rot[0], 1.0f))
                    selectedObject->transform.SetRotation(rot);
                
                ImGui::Spacing();
                ImGui::Separator();
                
                glm::vec3 scale = selectedObject->transform.GetScale();
                float uniformScale = scale.x;
                ImGui::Text("Scale");
                if (ImGui::DragFloat("##Scale", &uniformScale, 0.01f, 0.01f, 10.0f))
                    selectedObject->transform.SetScale(uniformScale);
            }

            ImGui::Separator();

            glm::vec3 color;
            if (ImGui::ColorEdit3("SetColor", &color[0]))
                CommandManager::ExecuteCommand("onChangeMeshColor", {color});

            ImGui::Separator();

            if (ImGui::CollapsingHeader("Fun Functions"))
            {
                ImGui::Text("there is no functional :(");
            }
            
            ImGui::Separator();
            
            if (ImGui::CollapsingHeader("Mesh Renderer"))
            {
                ImGui::BulletText("Type: Cube");
                ImGui::BulletText("Vertices: 24");
                ImGui::BulletText("Triangles: 12");
            }
            
            if (ImGui::CollapsingHeader("Material"))
            {
                ImGui::BulletText("Shader: basic");
                ImGui::BulletText("Diffuse: bricks2.jpg");
            }
        }
        else
        {
            ImGui::TextDisabled("No object selected");
            ImGui::Separator();
            ImGui::TextWrapped("Select an object from Hierarchy");
        }
        
        ImGui::End();
    }

    void RenderProperties(Camera* camera, Renderer* renderer)
    {
        ImGui::SetNextWindowPos({10, 580}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 250}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Scene Settings", &showProperties);

        if (ImGui::CollapsingHeader("Camera"))
        {
            float speed = camera->GetMovementSpeed();
            if (ImGui::SliderFloat("Speed", &speed, 1.f, 20.f))
                camera->SetMovementSpeed(speed);

            ImGui::Spacing();

            float sensitivity = camera->GetMouseSensitivity();
            if (ImGui::SliderFloat("Mouse Sensitivity", &sensitivity, 0.0f, 1.0f))
                camera->SetMouseSensitivity(sensitivity);

            ImGui::Spacing();

            float zoom = camera->GetZoom();
            if (ImGui::SliderFloat("Zoom", &zoom, 1.0f, 100.0f))
                camera->SetZoom(zoom);

            ImGui::Spacing();

            float pitch = camera->GetPitch();
            if (ImGui::SliderFloat("Pitch", &pitch, -90.0f, 90.0f, "%.1f"))
                camera->SetPitch(pitch);

            ImGui::Spacing();

            float yaw = camera->GetYaw();
            if (ImGui::SliderFloat("Yaw", &yaw, -360.0f, 360.0f, "%.1f"))
                camera->SetYaw(yaw);
        }

        if (ImGui::CollapsingHeader("Renderer"))
        {
            auto& s = renderer->GetSettings();
            ImGui::Checkbox("Wireframe", &s.enableWireframe);
            ImGui::Checkbox("Depth Test", &s.enableDepthTest);
            ImGui::ColorEdit3("Clear", &s.clearColor[0]);
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
