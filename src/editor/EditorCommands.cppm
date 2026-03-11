module;

#include <string>
#include <memory>
#include <variant>
#include <functional>

export module WFE.EditorCommands;

import WFE.Core.Logger;
import WFE.Core.CommandManager;
import WFE.ECS.ECSModule;
import WFE.Resource.ResourceModule;
import WFE.UI.UIModule;
import WFE.Scene.SceneSurializer;
import WFE.Rendering.Primitive.PrimitivesFactory;
import WFE.ECS.Components;
import WFE.Scene.Light;

import WFE.GameRuntime;

export class EditorCommands
{
private:
    ECSModule* ecsModule = nullptr;
    ResourceModule* resourceModule = nullptr;
    UIModule* uiModule = nullptr;
    SceneSerializer* sceneSerializer = nullptr;
    GameRuntime* runtime = nullptr;
    std::function<void()> exitCallback;

public:
    EditorCommands(ECSModule* ecs, ResourceModule* resource, UIModule* ui, SceneSerializer* serializer, GameRuntime* runtimePtr = nullptr, std::function<void()> exitCb = {})
        : ecsModule(ecs), resourceModule(resource), uiModule(ui), sceneSerializer(serializer), runtime(runtimePtr), exitCallback(exitCb)
    {}

    void RegisterAll()
    {
        using namespace WFE::Core;

        CommandManager::RegisterCommand("onCreateCube",
        [this](const CommandArgs&)
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Cube");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));

            auto cubeMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
            ecsModule->GetECS()->AddComponent<MeshComponent>(entity, cubeMesh);

            auto material = resourceModule->GetMaterialManager()->GetMaterial("gray");
            ecsModule->GetECS()->AddComponent<MaterialComponent>(entity, material);

            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<RotationComponent>(entity, 50.0f);

            Logger::Log(LogLevel::INFO, "Cube entity created");
        });

        CommandManager::RegisterCommand("onCreatePlane",
        [this](const CommandArgs&)
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Plane");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity, glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1));

            auto mesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::QUAD);
            ecsModule->GetECS()->AddComponent<MeshComponent>(entity, mesh);

            auto material = resourceModule->GetMaterialManager()->GetMaterial("gray");
            ecsModule->GetECS()->AddComponent<MaterialComponent>(entity, material);

            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<RotationComponent>(entity, 50.0f);

            Logger::Log(LogLevel::INFO, "Cube entity created");
        });

        

        CommandManager::RegisterCommand("onChangeMeshColor",
        [this](const CommandArgs& args)
        {
            if (args.size() != 1)
            {
                Logger::Log(LogLevel::ERROR, "onChangeMeshColor requires 1 argument: color");
                return;
            }

            const auto& color = std::get<glm::vec3>(args[0]);
            entt::entity selected = uiModule->GetEditorLayout()->GetSelectedEntity();

            if (selected == entt::null || !ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            if (ecsModule->GetECS()->HasComponent<ColorComponent>(selected))
            {
                auto& colorComp = ecsModule->GetECS()->GetComponent<ColorComponent>(selected);
                colorComp.color = color;
            }
            else
            {
                ecsModule->GetECS()->AddComponent<ColorComponent>(selected, color);
            }

            if (ecsModule->GetECS()->HasComponent<MeshComponent>(selected))
            {
                auto& meshComp = ecsModule->GetECS()->GetComponent<MeshComponent>(selected);
                if (meshComp.mesh)
                    meshComp.mesh->SetColor(color);
            }
        });

        CommandManager::RegisterCommand("onCreateDirectionalLight",
        [this](const CommandArgs&)
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Directional Light");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity,
                glm::vec3(0, 10, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::DIRECTIONAL);
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<IconComponent>(entity,
                "assets/textures/icons/light_directional.png", 0.3f);
            Logger::Log(LogLevel::INFO, "Directional light created with icon");
        });

        CommandManager::RegisterCommand("onCreatePointLight",
        [this](const CommandArgs&)
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Point Light");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity,
                glm::vec3(0, 5, 0), glm::vec3(0), glm::vec3(1));
            ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::POINT);
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<IconComponent>(entity,
                "assets/textures/icons/light_point.png", 0.4f);
            Logger::Log(LogLevel::INFO, "Point light created with icon");
        });

        CommandManager::RegisterCommand("onCreateSpotLight",
        [this](const CommandArgs&)
        {
            auto entity = ecsModule->GetECS()->CreateEntity("Spot Light");
            ecsModule->GetECS()->AddComponent<TransformComponent>(entity,
                glm::vec3(0, 5, 0), glm::vec3(45, 0, 0), glm::vec3(1));
            ecsModule->GetECS()->AddComponent<LightComponent>(entity, LightType::SPOT);
            ecsModule->GetECS()->AddComponent<VisibilityComponent>(entity, true);
            ecsModule->GetECS()->AddComponent<IconComponent>(entity,
                "assets/textures/icons/light_spot.png", 0.35f);
            Logger::Log(LogLevel::INFO, "Spot light created with icon");
        });

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

            bool success = false;
            if (sceneSerializer)
                success = sceneSerializer->SaveScene(filename);

            if (success)
                Logger::Log(LogLevel::INFO, "✓ Scene saved successfully: " + filename);
            else
                Logger::Log(LogLevel::ERROR, "✗ Failed to save scene: " + filename);
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

            bool success = false;
            if (sceneSerializer && resourceModule)
            {
                success = sceneSerializer->LoadScene(
                    filename,
                    resourceModule->GetMaterialManager(),
                    resourceModule->GetTextureManager()
                );
            }

            if (success)
                Logger::Log(LogLevel::INFO, "✓ Scene loaded successfully: " + filename);
            else
                Logger::Log(LogLevel::ERROR, "✗ Failed to load scene: " + filename);
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

        CommandManager::RegisterCommand("onListScenes",
        [this](const CommandArgs&)
        {
            if (!sceneSerializer)
            {
                Logger::Log(LogLevel::WARNING, "Scene serializer not available");
                return;
            }

            auto scenes = sceneSerializer->GetAvailableScenes();

            if (scenes.empty())
            {
                Logger::Log(LogLevel::INFO, "No saved scenes found");
            }
            else
            {
                Logger::Log(LogLevel::INFO, "Available scenes (" + std::to_string(scenes.size()) + "): ");
                for (const auto& scene : scenes)
                    Logger::Log(LogLevel::INFO, "  - " + scene);
            }
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

            if (!resourceModule->GetModelManager())
            {
                Logger::Log(LogLevel::ERROR, "ModelManager is NULL!");
                return;
            }

            if (!ecsModule->GetECS())
            {
                Logger::Log(LogLevel::ERROR, "ECSWorld is NULL!");
                return;
            }

            Logger::Log(LogLevel::INFO, "Calling LoadWithECS...");
            auto model = resourceModule->GetModelManager()->LoadWithECS(filepath, ecsModule->GetECS());

            if (!model) 
            {
                Logger::Log(LogLevel::ERROR, "Failed to load model: " + filepath);
                return;
            }

            Logger::Log(LogLevel::INFO, 
                "Model loaded successfully with " + std::to_string(model->GetMeshCount()) + " meshes");

            Logger::Log(LogLevel::INFO, "Total entities in world: " + std::to_string(ecsModule->GetECS()->GetEntityCount()));
            Logger::Log(LogLevel::INFO, "=== onLoadModel command complete ===\n");
        });

        CommandManager::RegisterCommand("onCreateCamera", [this](const CommandArgs&)
        {
            auto* ecs = ecsModule->GetECS();
            auto newCam = ecs->CreateCamera("Camera", false);

            auto view = ecs->GetRegistry().view<CameraComponent>();
            if (view.size() == 1)
            {
                ecs->GetComponent<CameraComponent>(newCam).isMainCamera = true;
                // Editor should set main camera via higher level if needed
            }

            Logger::Log(LogLevel::INFO, "New camera created and assigned as main if needed");
        });

        // Play / Stop commands delegate to GameRuntime when available
        CommandManager::RegisterCommand("onPlayGame",
        [this](const CommandArgs&)
        {
            if (!runtime)
            {
                Logger::Log(LogLevel::WARNING, "GameRuntime not available");
                return;
            }

            Logger::Log(LogLevel::INFO, "=== ENTERING PLAY MODE ===");
            runtime->Start();
            Logger::Log(LogLevel::INFO, "Play mode started");
        });

        CommandManager::RegisterCommand("onStopGame",
        [this](const CommandArgs&)
        {
            if (!runtime)
            {
                Logger::Log(LogLevel::WARNING, "GameRuntime not available");
                return;
            }

            Logger::Log(LogLevel::INFO, "=== EXITING PLAY MODE ===");
            runtime->Stop();
            Logger::Log(LogLevel::INFO, "Play mode stopped");
        });

        CommandManager::RegisterCommand("onAttachScript",
        [this](const CommandArgs& args)
        {
            if (args.empty())
            {
                Logger::Log(LogLevel::ERROR,
                    "onAttachScript requires script path");
                return;
            }

            entt::entity selected = uiModule->GetEditorLayout()->GetSelectedEntity();
            if (selected == entt::null || !ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            std::string scriptPath = std::get<std::string>(args[0]);

            auto* ecs = ecsModule->GetECS();

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
            entt::entity selected = uiModule->GetEditorLayout()->GetSelectedEntity();
            if (selected == entt::null || !ecsModule->GetECS()->IsValid(selected))
            {
                Logger::Log(LogLevel::WARNING, "No entity selected");
                return;
            }

            if (ecsModule->GetECS()->HasComponent<ScriptComponent>(selected))
            {
                auto& script = ecsModule->GetECS()->GetComponent<ScriptComponent>(selected);
                
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
                
                ecsModule->GetECS()->RemoveComponent<ScriptComponent>(selected);
                Logger::Log(LogLevel::INFO, "Script removed from entity");
            }
            else
            {
                Logger::Log(LogLevel::WARNING, "Entity has no script component");
            }
        });

        // Exit command: call exit callback when provided
        CommandManager::RegisterCommand("onExit",
        [this](const CommandArgs&)
        {
            Logger::Log(LogLevel::INFO, "Exit requested from menu");
            if (exitCallback) exitCallback();
        });

        // The following play/stop, attach/remove script related commands can be
        // registered here as well but may be delegated to a GameRuntime or
        // ScriptingManager for cleaner separation.
    }
};
