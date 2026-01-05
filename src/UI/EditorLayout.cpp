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

void EditorLayout::RenderEditor(SceneManager* sceneManager, Camera* camera, Renderer* renderer,
                                std::function<void()> onCreateCube)
{
    if (!sceneManager || !camera || !renderer)
    {
        Logger::Log(LogLevel::ERROR, "EditorLayout: nullptr passed to RenderEditor!");
        return;
    }
    
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
}

void EditorLayout::RenderControlPanel(std::function<void()> onCreateCube)
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 150), ImGuiCond_FirstUseEver);

    ImGuiWindowFlagSettings();
    
    ImGui::Begin("Control Panel");
    
    ImGui::Text("Scene Controls");
    ImGui::Separator();
    
    if (ImGui::Button("Create Cube", ImVec2(-1, 30)))
    {
        if (onCreateCube) 
            onCreateCube();

        Logger::Log(LogLevel::INFO, "Cube was created");
    }
    
    if (ImGui::Button("Create Sphere", ImVec2(-1, 30)))
        Logger::Log(LogLevel::INFO, "Sphere not implemented yet");
    
    if (ImGui::Button("Clear Scene", ImVec2(-1, 30)))
        Logger::Log(LogLevel::INFO, "Clear not implemented yet");
    
    ImGui::Separator();

    ImGui::Checkbox("Show Console", &showConsole);
    
    ImGui::End();
}

void EditorLayout::RenderMenuBar(std::function<void()> onCreateCube)
{
    return;
}

void EditorLayout::RenderViewport()
{
    ImGui::SetNextWindowPos(ImVec2(300, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(800, 650), ImGuiCond_FirstUseEver);

    ImGuiWindowFlagSettings();
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Game Viewport");
    
    isViewportHovered = ImGui::IsWindowHovered();
    isViewportFocused = ImGui::IsWindowFocused();
    
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    
    if ((windowSize.x != viewportSize.x || windowSize.y != viewportSize.y) && 
        windowSize.x > 0 && windowSize.y > 0)
    {
        viewportSize = windowSize;
        framebuffer->Resize((int)viewportSize.x, (int)viewportSize.y);
        Logger::Log(LogLevel::INFO, "Viewport resized: " + 
            std::to_string((int)viewportSize.x) + "x" + std::to_string((int)viewportSize.y));
    }
    
    viewportPos = ImGui::GetCursorScreenPos();
    
    if (framebuffer)
    {
        unsigned int texID = framebuffer->GetTextureID();   
        ImGui::Image((void*)(intptr_t)texID, windowSize, ImVec2(0, 1), ImVec2(1, 0)   );
    }
    else
        ImGui::Text("Framebuffer Error!");
    
    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorLayout::RenderSceneHierarchy(SceneManager* sceneManager)
{
    ImGuiWindowFlagSettings();
    
    if (!sceneManager)
    {
        Logger::Log(LogLevel::ERROR, "SceneManager is null!");
        return;
    }
    
    ImGui::SetNextWindowPos(ImVec2(10, 170), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 400), ImGuiCond_FirstUseEver);

    ImGuiWindowFlagSettings();
    
    ImGui::Begin("Hierarchy", &showHierarchy);
    
    size_t objCount = sceneManager->GetObjectCount();
    ImGui::Text("Objects: %zu", objCount);
    ImGui::Separator();

    uint64_t objectToDelete = 0;
    
    try
    {
        auto& objects = sceneManager->GetObjects();
        
        for (auto& pair : objects)
        {
            uint64_t id = pair.first;
            auto& objPtr = pair.second;
            
            if (!objPtr)
                continue;
            
            SceneObject* obj = objPtr.get();
            
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            
            if (selectedObject == obj)
                flags |= ImGuiTreeNodeFlags_Selected;
            
            std::string objInfo = obj->name + " [" + std::to_string(obj->objectID) + "]";
            ImGui::TreeNodeEx((void*)(intptr_t)id, flags, objInfo.c_str());
            
            if (ImGui::IsItemClicked())
            {
                selectedObject = obj;
                Logger::Log(LogLevel::INFO, "Selected: " + obj->name);
            }
            
            if (ImGui::BeginPopupContextItem())
            {
                ImGui::Text("Object: %s", obj->name.c_str());
                ImGui::Separator();
                
                if (ImGui::MenuItem("Delete"))
                {
                    objectToDelete = obj->objectID;
                    Logger::Log(LogLevel::INFO, "Deleting: " + obj->name);
                }
                
                ImGui::EndPopup();
            }
        }

    }
    catch (const std::exception& e)
    {
        Logger::Log(LogLevel::ERROR, "Exception in Hierarchy: " + std::string(e.what()));
    }

    if (objectToDelete != 0)
    {
        sceneManager->RemoveObject(objectToDelete);
        Logger::Log(LogLevel::INFO, "Object was successful deleted with ID: " + std::to_string(objectToDelete));
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
    ImGui::SetNextWindowPos(ImVec2(1110, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 660), ImGuiCond_FirstUseEver);

    ImGuiWindowFlagSettings();
    
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

void EditorLayout::RenderProperties(Camera* camera, Renderer* renderer)
{
    if (!camera || !renderer)
        return;
    
    ImGui::SetNextWindowPos(ImVec2(10, 580), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(280, 250), ImGuiCond_FirstUseEver);

    ImGuiWindowFlagSettings();
    
    ImGui::Begin("Scene Settings", &showProperties);
    
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
    ImGui::SetNextWindowPos(ImVec2(300, 670), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(800, 120), ImGuiCond_FirstUseEver);

    ImGuiWindowFlagSettings();
    
    ImGui::Begin("Console", &showConsole);
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[INFO] Application started");
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "[SUCCESS] Scene initialized");
    ImGui::Separator();
    ImGui::Text("Ready.");
    
    ImGui::End();
}

void EditorLayout::ImGuiWindowFlagSettings() 
{
//ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
}