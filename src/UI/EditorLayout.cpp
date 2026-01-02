#include "UI/EditorLayout.h"
#include "utils/Logger.h"

#include <imgui.h>
#include <cstring>

EditorLayout::EditorLayout()
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
    Logger::Log(LogLevel::INFO, "EditorLayout created with Framebuffer");
}

void EditorLayout::BeginFrame()
{
}

void EditorLayout::EndFrame()
{
}

ImVec2 EditorLayout::GetViewportSize() const
{
    return viewportSize;
}

ImVec2 EditorLayout::GetViewportPos() const
{
    return viewportPos;
}

void EditorLayout::RenderEditor(SceneManager* sceneManager, Camera* camera, Renderer* renderer,
                                std::function<void()> onCreateCube)
{
    if (!sceneManager || !camera || !renderer)
    {
        Logger::Log(LogLevel::ERROR, "EditorLayout: nullptr passed to RenderEditor!");
        return;
    }
    // Create a full-screen dockspace and initial layout on first use
    ImGuiIO& io = ImGui::GetIO();
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGuiWindowFlags hostWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("EditorDockHost", nullptr, hostWindowFlags);
    ImGui::PopStyleVar(2);

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");

        static bool dockInitialized = false;
        if (!dockInitialized)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

            ImGuiID dock_main_id = dockspace_id;
            ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
            ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

            ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
            ImGui::DockBuilderDockWindow("Control Panel", dock_left_id);
            ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
            ImGui::DockBuilderDockWindow("Scene Settings", dock_bottom_id);
            ImGui::DockBuilderDockWindow("Console", dock_bottom_id);
            ImGui::DockBuilderDockWindow("Viewport", dock_main_id);

            ImGui::DockBuilderFinish(dockspace_id);
            dockInitialized = true;
            Logger::Log(LogLevel::INFO, "Dockspace initialized");
        }

        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
    }

    // Render child windows (they will be docked into the DockSpace)
    RenderControlPanel(onCreateCube);

    if (showHierarchy)
        RenderSceneHierarchy(sceneManager);

    if (showInspector)
        RenderInspector();

    if (showProperties)
        RenderProperties(camera, renderer);

    if (showConsole)
        RenderConsole();

    RenderViewport();

    ImGui::End(); // EditorDockHost
}

void EditorLayout::RenderControlPanel(std::function<void()> onCreateCube)
{
    ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Control Panel", nullptr, panelFlags);
    
    ImGui::Text("Scene Controls");
    ImGui::Separator();
    
    if (ImGui::Button("Create Cube", ImVec2(-1, 30)))
    {
        Logger::Log(LogLevel::INFO, "Creating cube...");
        if (onCreateCube) 
            onCreateCube();
    }
    
    if (ImGui::Button("Create Sphere", ImVec2(-1, 30)))
    {
        Logger::Log(LogLevel::INFO, "Sphere not implemented yet");
    }
    
    if (ImGui::Button("Clear Scene", ImVec2(-1, 30)))
    {
        Logger::Log(LogLevel::INFO, "Clear not implemented yet");
    }
    
    ImGui::Separator();
    
    ImGui::Checkbox("Show Console", &showConsole);
    
    ImGui::End();
}

void EditorLayout::RenderMenuBar(std::function<void()> onCreateCube)
{
    // Не використовуємо - замість цього використовуємо Control Panel
}

void EditorLayout::RenderViewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGuiWindowFlags viewportFlags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Viewport", nullptr, viewportFlags);
    
    isViewportHovered = ImGui::IsWindowHovered();
    isViewportFocused = ImGui::IsWindowFocused();
    
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    
    static bool logged = false;
    if (!logged)
    {
        Logger::Log(LogLevel::INFO, "Viewport window size: " + 
            std::to_string((int)windowSize.x) + "x" + std::to_string((int)windowSize.y));
        logged = true;
    }
    
    if (windowSize.x != viewportSize.x || windowSize.y != viewportSize.y)
    {
        viewportSize = windowSize;
        
        if (viewportSize.x > 0 && viewportSize.y > 0)
        {
            framebuffer->Resize((int)viewportSize.x, (int)viewportSize.y);
            Logger::Log(LogLevel::INFO, "Viewport resized to: " + 
                std::to_string((int)viewportSize.x) + "x" + std::to_string((int)viewportSize.y));
        }
    }
    
    viewportPos = ImGui::GetCursorScreenPos();
    
    if (framebuffer)
    {
        unsigned int texID = framebuffer->GetTextureID();
        
        ImGui::Image(
            (void*)(intptr_t)texID,
            windowSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    }
    else
    {
        Logger::Log(LogLevel::ERROR, "Framebuffer is null in RenderViewport!");
        ImGui::Text("Framebuffer Error!");
    }
    
    ImGui::End();
    ImGui::PopStyleVar();
}


void EditorLayout::RenderSceneHierarchy(SceneManager* sceneManager)
{
    if (!sceneManager)
    {
        Logger::Log(LogLevel::ERROR, "SceneManager is null!");
        return;
    }
    
    ImGuiWindowFlags hierarchyFlags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Hierarchy", &showHierarchy, hierarchyFlags);
    
    size_t objCount = sceneManager->GetObjectCount();
    ImGui::Text("Objects: %zu", objCount);
    ImGui::Separator();
    
    try
    {
        auto& objects = sceneManager->GetObjects();
        
        for (auto& pair : objects)
        {
            uint64_t id = pair.first;
            auto& objPtr = pair.second;
            
            if (!objPtr)
            {
                Logger::Log(LogLevel::WARNING, "Null object in scene!");
                continue;
            }
            
            SceneObject* obj = objPtr.get();
            
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            
            if (selectedObject == obj)
                flags |= ImGuiTreeNodeFlags_Selected;
            
            // Іконка + назва
            ImGui::TreeNodeEx((void*)(intptr_t)id, flags, "[Cube] %s", obj->name.c_str());
            
            if (ImGui::IsItemClicked())
            {
                selectedObject = obj;
                Logger::Log(LogLevel::INFO, "Selected: " + obj->name);
            }
            
            // Правий клік - контекстне меню
            if (ImGui::BeginPopupContextItem())
            {
                ImGui::Text("Object: %s", obj->name.c_str());
                ImGui::Separator();
                
                if (ImGui::MenuItem("Delete"))
                {
                    Logger::Log(LogLevel::INFO, "Deleting: " + obj->name);
                    sceneManager->RemoveObject(id);
                    if (selectedObject == obj)
                        selectedObject = nullptr;
                }
                
                if (ImGui::MenuItem("Duplicate"))
                {
                    Logger::Log(LogLevel::INFO, "Duplicate not implemented");
                }
                
                ImGui::EndPopup();
            }
        }
    }
    catch (const std::exception& e)
    {
        Logger::Log(LogLevel::ERROR, "Exception in Hierarchy: " + std::string(e.what()));
    }
    
    if (objCount == 0)
    {
        ImGui::Spacing();
        ImGui::TextDisabled("(Empty Scene)");
        ImGui::TextWrapped("Use Control Panel to create objects");
    }
    
    ImGui::End();
}

void EditorLayout::RenderInspector()
{
    ImGuiWindowFlags inspectorFlags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Inspector", &showInspector, inspectorFlags);
    
    if (selectedObject)
    {
        // Назва об'єкта
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
        
        // Transform
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            glm::vec3 pos = selectedObject->transform.GetPosition();
            ImGui::Text("Position");
            if (ImGui::DragFloat3("##Pos", &pos[0], 0.1f))
                selectedObject->transform.SetPosition(pos);
            
            ImGui::Spacing();
            
            glm::vec3 rot = selectedObject->transform.GetRotation();
            ImGui::Text("Rotation");
            if (ImGui::DragFloat3("##Rot", &rot[0], 1.0f))
                selectedObject->transform.SetRotation(rot);
            
            ImGui::Spacing();
            
            glm::vec3 scale = selectedObject->transform.GetScale();
            float uniformScale = scale.x;
            ImGui::Text("Scale");
            if (ImGui::DragFloat("##Scale", &uniformScale, 0.01f, 0.01f, 10.0f))
                selectedObject->transform.SetScale(uniformScale);
        }
        
        ImGui::Separator();
        
        // Mesh Info
        if (ImGui::CollapsingHeader("Mesh Renderer"))
        {
            ImGui::BulletText("Type: Cube");
            ImGui::BulletText("Vertices: 24");
            ImGui::BulletText("Triangles: 12");
        }
        
        // Material
        if (ImGui::CollapsingHeader("Material"))
        {
            ImGui::BulletText("Shader: parallax");
            ImGui::BulletText("Diffuse: bricks2.jpg");
            ImGui::BulletText("Normal: bricks2_normal.jpg");
            ImGui::BulletText("Height: bricks2_disp.jpg");
        }
    }
    else
    {
        ImGui::TextDisabled("No object selected");
        ImGui::Separator();
        ImGui::TextWrapped("Select an object from the Hierarchy panel");
    }
    
    ImGui::End();
}

void EditorLayout::RenderProperties(Camera* camera, Renderer* renderer)
{
    if (!camera || !renderer)
    {
        Logger::Log(LogLevel::ERROR, "Null camera or renderer!");
        return;
    }
    
    ImGuiWindowFlags propertiesFlags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Scene Settings", &showProperties, propertiesFlags);
    
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        glm::vec3 pos = camera->GetPosition();
        ImGui::Text("Pos: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        
        float speed = camera->GetMovementSpeed();
        if (ImGui::SliderFloat("Speed", &speed, 1.0f, 20.0f))
            camera->SetMovementSpeed(speed);
        
        float sens = camera->GetMouseSensitivity();
        if (ImGui::SliderFloat("Sensitivity", &sens, 0.01f, 1.0f))
            camera->SetMouseSensitivity(sens);
        
        if (ImGui::Button("Reset Camera", ImVec2(-1, 0)))
            camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    }
    
    if (ImGui::CollapsingHeader("Renderer"))
    {
        auto& settings = renderer->GetSettings();
        
        ImGui::Checkbox("Wireframe", &settings.enableWireframe);
        ImGui::Checkbox("Depth Test", &settings.enableDepthTest);
        
        ImGui::Text("Clear Color");
        ImGui::ColorEdit3("##Clear", &settings.clearColor[0]);
    }
    
    ImGui::End();
}

void EditorLayout::RenderConsole()
{
    ImGuiWindowFlags consoleFlags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Console", &showConsole, consoleFlags);
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[INFO] Application started");
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "[SUCCESS] Scene initialized");
    ImGui::Separator();
    ImGui::Text("Ready.");
    
    ImGui::End();
}