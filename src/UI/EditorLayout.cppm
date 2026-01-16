module; 

#include <glm/glm.hpp>
#include <imgui.h>
#include <entt.hpp>

#include <string>
#include <cstring>
#include <memory>
#include <unordered_map>

export module XEngine.UI.EditorLayout;

import XEngine.ECS.ECSWorld;
import XEngine.ECS.Components;

import XEngine.Core.Camera;
import XEngine.Core.Logger;
import XEngine.Core.CommandManager;

import XEngine.Rendering.Renderer;
import XEngine.Rendering.Framebuffer;

import XEngine.UI.Theme; 

import XEngine.Resource.Shader.ShaderManager;

export class EditorLayout
{
public:
    EditorLayout()
        : selectedEntity(entt::null)
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

    void RenderEditor(ECSWorld* ecs, Camera* camera, Renderer* renderer, ShaderManager* shaderManager)
    {
        if (!ecs || !camera || !renderer)
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

        if (showHierarchy)  RenderSceneHierarchy(ecs, shaderManager);
        if (showInspector)  RenderInspector(ecs, shaderManager);
        if (showProperties) RenderProperties(camera, renderer);
        if (showConsole)    RenderConsole();
        
        RenderViewport();
    }

    ImVec2 GetViewportSize()  const { return viewportSize; }
    ImVec2 GetViewportPos()   const { return viewportPos; }
    bool   IsViewportHovered() const { return isViewportHovered; }
    bool   IsViewportFocused() const { return isViewportFocused; }

    entt::entity GetSelectedEntity() const { return selectedEntity; }
    ShaderObj* GetSelectedShader() const { return selectedShader; }
    Framebuffer* GetFramebuffer() const { return framebuffer.get(); }
    void SetSelectedEntity(entt::entity entity) { selectedEntity = entity; }

private:
    // ───── state ─────

    entt::entity selectedEntity;
    ShaderObj* selectedShader;

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

    void RenderSceneHierarchy(ECSWorld* ecs, ShaderManager* shaderManager)
    {
        ImGui::SetNextWindowPos({10, 170}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280, 400}, ImGuiCond_FirstUseEver);

        ImGui::Begin("Hierarchy", &showHierarchy);

        entt::entity entityToDelete = entt::null;

        if (ImGui::CollapsingHeader("Objects"))
        {
            ImGui::Text("Entities: %zu", ecs->GetEntityCount());
            ImGui::Separator();

            ecs->Each<TagComponent, IDComponent>(
                [&](entt::entity entity, TagComponent& tag, IDComponent& id) 
            {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

                if (selectedEntity == entity)
                    flags |= ImGuiTreeNodeFlags_Selected;

                std::string label = tag.name + " [" + std::to_string(id.id) + "]";

                ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, "%s", label.c_str());

                if (ImGui::IsItemClicked())
                    selectedEntity = entity;

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete"))
                        entityToDelete = entity;

                    ImGui::EndPopup();
                }
            });
        }

        if (ImGui::CollapsingHeader("Shaders"))
        {
            ImGui::Text("Shaders: %zu", shaderManager->GetCount());
            ImGui::Separator();

            for (auto& [name, shader] : shaderManager->GetShaderMap())
            {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

                if (selectedShader == shader.get())
                    flags |= ImGuiTreeNodeFlags_Selected;

                std::string label = name + " [" + std::to_string(shader->ID) + "]";

                ImGui::TreeNodeEx((void*)(intptr_t)shader.get(), flags, "%s", label.c_str());

                if (ImGui::IsItemClicked())
                    selectedShader = shader.get();
            }
        }

        if (entityToDelete != entt::null)
        {
            ecs->DestroyEntity(entityToDelete);
            if (selectedEntity == entityToDelete)
                selectedEntity = entt::null;
        }

        ImGui::End();
    }

    void RenderInspector(ECSWorld* ecs, ShaderManager* shaderManager)
    {
        ImGui::SetNextWindowPos(ImVec2(1110, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 660), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Inspector", &showInspector);
        
        if (selectedEntity != entt::null && ecs->IsValid(selectedEntity))
        {
            ImGui::Text("Entity");
            ImGui::Separator();
            
            if (ecs->HasComponent<TagComponent>(selectedEntity))
            {
                auto& tag = ecs->GetComponent<TagComponent>(selectedEntity);
                
                char buffer[256];
                strncpy(buffer, tag.name.c_str(), sizeof(buffer));
                buffer[255] = '\0';
                
                ImGui::Text("Name:");
                ImGui::SameLine();
                ImGui::PushItemWidth(-1);
                if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
                    tag.name = std::string(buffer);
                ImGui::PopItemWidth();
            }
            
            ImGui::Spacing();
            
            if (ecs->HasComponent<VisibilityComponent>(selectedEntity))
            {
                auto& vis = ecs->GetComponent<VisibilityComponent>(selectedEntity);
                ImGui::Checkbox("Active", &vis.isActive);
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            
            if (ecs->HasComponent<TransformComponent>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto& transform = ecs->GetComponent<TransformComponent>(selectedEntity);
                    
                    ImGui::Text("Position");
                    ImGui::DragFloat3("##Pos", &transform.position[0], 0.1f);
                    
                    ImGui::Spacing();
                    ImGui::Separator();
                    
                    ImGui::Text("Rotation");
                    ImGui::DragFloat3("##Rot", &transform.rotation[0], 1.0f);
                    
                    ImGui::Spacing();
                    ImGui::Separator();
                    
                    ImGui::Text("Scale");
                    float uniformScale = transform.scale.x;
                    if (ImGui::DragFloat("##Scale", &uniformScale, 0.01f, 0.01f, 10.0f))
                        transform.scale = glm::vec3(uniformScale);
                }
            }

            if (ecs->HasComponent<ColorComponent>(selectedEntity))
            {
                auto& colorComp = ecs->GetComponent<ColorComponent>(selectedEntity);
                
                static std::unordered_map<entt::entity, float[3]> entityColors;
                static std::unordered_map<entt::entity, bool> entityColorsInitialized;
                
                if (!entityColorsInitialized[selectedEntity])
                {
                    entityColors[selectedEntity][0] = colorComp.color.r;
                    entityColors[selectedEntity][1] = colorComp.color.g;
                    entityColors[selectedEntity][2] = colorComp.color.b;
                    entityColorsInitialized[selectedEntity] = true;
                }
                
                float* colorEdit = entityColors[selectedEntity];
                
                ImGui::Text("Color");
                if (ImGui::ColorEdit3("##Color", colorEdit))
                {
                    glm::vec3 newColor(colorEdit[0], colorEdit[1], colorEdit[2]);
                    
                    if (CommandManager::HasCommand("onChangeMeshColor"))
                        CommandManager::ExecuteCommand("onChangeMeshColor", {newColor});
                }
            }

            if (ecs->HasComponent<RotationComponent>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Rotation Animation"))
                {
                    auto& rotation = ecs->GetComponent<RotationComponent>(selectedEntity);
                    
                    ImGui::Checkbox("Auto Rotate", &rotation.autoRotate);
                    ImGui::DragFloat("Speed", &rotation.speed, 1.0f, 0.0f, 360.0f);
                    ImGui::DragFloat3("Axis", &rotation.axis[0], 0.1f, -1.0f, 1.0f);
                }
            }
            else
            {
                if (ImGui::Button("Add Rotation Component"))
                    ecs->AddComponent<RotationComponent>(selectedEntity);
            }
            
            if (ecs->HasComponent<MeshComponent>(selectedEntity))
                if (ImGui::CollapsingHeader("Mesh Renderer"))
                {
                    ImGui::BulletText("Type: Mesh");
                    ImGui::BulletText("Has Mesh: Yes");
                }
            
            if (ecs->HasComponent<MaterialComponent>(selectedEntity))
                if (ImGui::CollapsingHeader("Material"))
                    ImGui::BulletText("Has Material: Yes"); 
        }
        else if (selectedShader != nullptr && shaderManager->IsShaderValid(selectedShader->name))
        {
            if (ImGui::MenuItem("Reload"))
                shaderManager->Reload(selectedShader->name);
        }
        else
        {
            ImGui::TextDisabled("Nothink selected");
            ImGui::Separator();
            ImGui::TextWrapped("Select an object from Hierarchy or stfu");
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
        ImGui::Text("[INFO] Editor ready with ECS");
        ImGui::End();
    }
};