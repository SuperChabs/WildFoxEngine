module;

#include <string>
#include <variant> 

#include <glm/glm.hpp>
#include <entt/entt.hpp>

export module WFE.Engine.EditorCommandHandler;

import WFE.ECS.ECSModule;
import WFE.Resource.ResourceModule;
import WFE.UI.UIModule;
import WFE.Scene.SceneModule;
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.ECS.Components;
import WFE.Core.Logger;
import WFE.Core.CommandManager;
import WFE.Core.ModuleManager;
import WFE.Scene.Light;

export class EditorCommandHandler
{
    ModuleManager* m_moduleManger;

    ECSModule* m_ecsModule;
    ResourceModule* m_resModule;
    UIModule* m_uiModule;
    SceneModule* m_sceneModule;

public:
    EditorCommandHandler(ModuleManager* mm)
        : m_moduleManger(mm)
    {
        m_sceneModule = m_moduleManger->GetModule<SceneModule>("Scene");
        m_ecsModule   = m_moduleManger->GetModule<ECSModule>("ECS");
        m_resModule   = m_moduleManger->GetModule<ResourceModule>("Resource");
        m_uiModule    = m_moduleManger->GetModule<UIModule>("UI");
    }

    void RegisterAllCommands()
    {
        RegisterObjectCommands();
        RegisterLightCommands();
        RegisterSceneCommands();
        RegisterScriptCommands();
    }

private:
    void RegisterObjectCommands()
    {
        CommandManager::RegisterCommand("onCreateCube",
        [this](const CommandArgs&) 
        {
            auto entity = m_ecsModule->GetECS()->CreateEntity("Cube");
            m_ecsModule->GetECS()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));
            
            auto cubeMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
            m_ecsModule->GetECS()->AddComponent<MeshComponent>(entity, cubeMesh);
            
            auto material = m_resModule->GetMaterialManager()->GetMaterial("gray");
            m_ecsModule->GetECS()->AddComponent<MaterialComponent>(entity, material);
            
            m_ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            m_ecsModule->GetECS()->AddComponent<RotationComponent>(entity, 50.0f);
            
            Logger::Log(LogLevel::INFO, "Cube entity created");
        });

        CommandManager::RegisterCommand("onCreatePlane",
        [this](const CommandArgs&) 
        {
            auto entity = m_ecsModule->GetECS()->CreateEntity("Plane");
            m_ecsModule->GetECS()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));
            
            auto mesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::QUAD);
            m_ecsModule->GetECS()->AddComponent<MeshComponent>(entity, mesh);
            
            auto material = m_resModule->GetMaterialManager()->GetMaterial("gray");
            m_ecsModule->GetECS()->AddComponent<MaterialComponent>(entity, material);
            
            m_ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            m_ecsModule->GetECS()->AddComponent<RotationComponent>(entity, 50.0f);
            
            Logger::Log(LogLevel::INFO, "Cube entity created");
        });

        CommandManager::RegisterCommand("onLoadModel",
        [this](const CommandArgs& args) 
        {
            Logger::Log(LogLevel::INFO, "=== onLoadModel command called ===");
            
            if (args.empty())
            {
                Logger::Log(LogLevel::ERROR, "onLoadModel requires filepath argument");
                return;
            }
            
            std::string filepath = std::get<std::string>(args[0]);
            Logger::Log(LogLevel::INFO, "Filepath: " + filepath);
            
            if (!m_resModule->GetModelManager())
            {
                Logger::Log(LogLevel::ERROR, "ModelManager is NULL!");
                return;
            }
            
            if (!m_ecsModule->GetECS())
            {
                Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
                return;
            }
            
            Logger::Log(LogLevel::INFO, "Calling LoadWithECS...");
            auto model = m_resModule->GetModelManager()->LoadWithECS(filepath, m_ecsModule->GetECS());
            
            if (!model) 
            {
                Logger::Log(LogLevel::ERROR, "Failed to load model: " + filepath);
                return;
            }
            
            Logger::Log(LogLevel::INFO, 
                "Model loaded successfully with " + std::to_string(model->GetMeshCount()) + " meshes");
            
            Logger::Log(LogLevel::INFO, "Total entities in world: " + std::to_string(m_ecsModule->GetECS()->GetEntityCount()));
            Logger::Log(LogLevel::INFO, "=== onLoadModel command complete ===\n");
        });

        CommandManager::RegisterCommand("onCreateCamera", [this](const CommandArgs&) 
        {
            auto* ecs = m_ecsModule->GetECS();
            auto newCam = ecs->CreateCamera("Camera", false);
            
            auto view = ecs->GetRegistry().view<CameraComponent>();
            if (view.size() == 1)
            {
                ecs->GetComponent<CameraComponent>(newCam).isMainCamera = true;
                //SetMainCameraEntity(newCam);
            }
            
            Logger::Log(LogLevel::INFO, "New camera created and assigned as main if needed");
        });
    }

    void RegisterLightCommands()
    {
        CommandManager::RegisterCommand("onCreateDirectionalLight",
        [this](const CommandArgs&) 
        {
            auto entity = m_ecsModule->GetECS()->CreateEntity("Directional Light");
            m_ecsModule->GetECS()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 10, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            m_ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::DIRECTIONAL);
            m_ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            m_ecsModule->GetECS()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_directional.png", 0.3f);
            Logger::Log(LogLevel::INFO, "Directional light created with icon");
        });

        CommandManager::RegisterCommand("onCreatePointLight",
        [this](const CommandArgs&) 
        {
            auto entity = m_ecsModule->GetECS()->CreateEntity("Point Light");
            m_ecsModule->GetECS()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 5, 0), glm::vec3(0), glm::vec3(1));
            m_ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::POINT);
            m_ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            m_ecsModule->GetECS()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_point.png", 0.4f);
            Logger::Log(LogLevel::INFO, "Point light created with icon");
        });

        CommandManager::RegisterCommand("onCreateSpotLight",
        [this](const CommandArgs&) 
        {
            auto entity = m_ecsModule->GetECS()->CreateEntity("Spot Light");
            m_ecsModule->GetECS()->AddComponent<TransformComponent>(entity, 
                glm::vec3(0, 5, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            m_ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::SPOT);
            m_ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            m_ecsModule->GetECS()->AddComponent<IconComponent>(entity, 
                "assets/textures/icons/light_spot.png", 0.35f);
            Logger::Log(LogLevel::INFO, "Spot light created with icon");
        });
    }

    void RegisterSceneCommands()
    {
        CommandManager::RegisterCommand("onSaveScene",
        [this](const CommandArgs& args) 
        {
            std::string filename = "scene.json";
            
            if (!args.empty())
            {
                try 
                {
                    filename = std::get<std::string>(args[0]);
                }
                catch (...) 
                {
                    Logger::Log(LogLevel::WARNING, "Invalid filename argument, using default");
                }
            }
            
            if (filename.find(".json") == std::string::npos)
                filename += ".json";
            
            bool success = m_sceneModule->GetSceneSerializer()->SaveScene(filename);
            
            if (success)
                Logger::Log(LogLevel::INFO, "Scene saved successfully: " + filename);
            else
                Logger::Log(LogLevel::ERROR, "Failed to save scene: " + filename);
        });
        
        CommandManager::RegisterCommand("onLoadScene",
        [this](const CommandArgs& args) 
        {
            std::string filename = "scene.json";
            
            if (!args.empty())
            {
                try 
                {
                    filename = std::get<std::string>(args[0]);
                }
                catch (...) 
                {
                    Logger::Log(LogLevel::WARNING, "Invalid filename argument, using default");
                }
            }
            
            if (filename.find(".json") == std::string::npos)
                filename += ".json";
            
            bool success = m_sceneModule->GetSceneSerializer()->LoadScene(
                filename,
                m_resModule->GetMaterialManager(),
                m_resModule->GetTextureManager()
            );
            
            if (success)
                Logger::Log(LogLevel::INFO, "Scene loaded successfully: " + filename);
            else
                Logger::Log(LogLevel::ERROR, "Failed to load scene: " + filename);
        });
        
        CommandManager::RegisterCommand("onQuickSave",
        [this](const CommandArgs&) 
        {
            CommandManager::ExecuteCommand("onSaveScene", {std::string("quicksave.json")});
        });
        
        CommandManager::RegisterCommand("onQuickLoad",
        [this](const CommandArgs&) 
        {
            CommandManager::ExecuteCommand("onLoadScene", {std::string("quicksave.json")});
        });

        // CommandManager::RegisterCommand("onNewScene",
        // [this](const CommandArgs&) 
        // {
        //     ecsModule->GetECS()->Clear();
        //    
        //     GetCamera()->SetPosition(glm::vec3(0, 0, 0));
        //     GetCamera()->SetYaw(-95.0f);
        //     GetCamera()->SetPitch(0.0f);
        //  
        //     Logger::Log(LogLevel::INFO, "New scene created");
        // });
        
        CommandManager::RegisterCommand("onListScenes",
        [this](const CommandArgs&) 
        {
            auto scenes = m_sceneModule->GetSceneSerializer()->GetAvailableScenes();
            
            if (scenes.empty())
            {
                Logger::Log(LogLevel::INFO, "No saved scenes found");
            }
            else
            {
                Logger::Log(LogLevel::INFO, "Available scenes (" + 
                    std::to_string(scenes.size()) + "):");
                
                for (const auto& scene : scenes)
                    Logger::Log(LogLevel::INFO, "  - " + scene);
            }
        });
    }

    void RegisterScriptCommands()
    {
        CommandManager::RegisterCommand("onAttachScript",
        [this](const CommandArgs& args)
        {
            if (args.empty())
            {
                Logger::Log(LogLevel::ERROR,
                    "onAttachScript requires script path");
                return;
            }

            entt::entity selected = m_uiModule->GetEditorLayout()->GetSelectedEntity();
            if (selected == entt::null || !m_ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            std::string scriptPath = std::get<std::string>(args[0]);

            auto* ecs = m_ecsModule->GetECS();

            if (ecs->HasComponent<ScriptComponent>(selected))
            {
                auto& script = ecs->GetComponent<ScriptComponent>(selected);

                script.scriptPath = scriptPath;
                script.loaded = false;
                script.failed = false;
                script.env = {};

                Logger::Log(LogLevel::INFO,
                    "Script updated: " + scriptPath);
            }
            else
            {
                auto& script = ecs->AddComponent<ScriptComponent>(selected);

                script.scriptPath = scriptPath;
                script.loaded = false;
                script.failed = false;

                Logger::Log(LogLevel::INFO,
                    "Script attached: " + scriptPath);
            }
        });

        CommandManager::RegisterCommand("onRemoveScript",
        [this](const CommandArgs&) 
        {
            entt::entity selected = m_uiModule->GetEditorLayout()->GetSelectedEntity();
            if (selected == entt::null || !m_ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            if (m_ecsModule->GetECS()->HasComponent<ScriptComponent>(selected))
            {
                auto& script = m_ecsModule->GetECS()->GetComponent<ScriptComponent>(selected);
                
                if (script.loaded && script.env["onDestroy"].valid())
                {
                    try 
                    {
                        script.env["onDestroy"]();
                    } 
                    catch (...) 
                    {
                        Logger::Log(LogLevel::WARNING, "Error in script onDestroy");
                    }
                }
                
                m_ecsModule->GetECS()->RemoveComponent<ScriptComponent>(selected);
                Logger::Log(LogLevel::INFO, "Script removed from entity");
            }
            else
            {
                Logger::Log(LogLevel::WARNING, "Entity has no script component");
            }
        });
    }

};