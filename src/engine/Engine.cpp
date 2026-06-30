#include "Engine.h"
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include "core/Input.h"
#include "core/CommandManager.h"
#include "core/logging/Logger.h"

void Engine::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (app && app->GetWindow())
        app->GetWindow()->SetSize(width, height);
}

void Engine::MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    engine->GetInput()->UpdateMousePosition(xpos, ypos);

    if (ImGuizmo::IsUsing() || ImGui::GetIO().WantCaptureMouse)
        return;

    if (engine->cameraControlEnabled && engine->mainCameraEntity != entt::null) {
        glm::vec2 delta = engine->GetInput()->GetMouseDelta();
        auto &orientation = engine->ecsModule->GetECS()->GetComponent<CameraOrientationComponent>(
            engine->mainCameraEntity);
        auto &config = engine->ecsModule->GetECS()->GetComponent<CameraComponent>(engine->mainCameraEntity);

        orientation.yaw += delta.x * config.mouseSensitivity;
        orientation.pitch += delta.y * config.mouseSensitivity;

        if (orientation.pitch > 89.0f) orientation.pitch = 89.0f;
        if (orientation.pitch < -89.0f) orientation.pitch = -89.0f;
    }
}

void Engine::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        if (engine->ShouldAllowCameraControl())
            engine->SetCameraControlMode(!engine->cameraControlEnabled);
}

void Engine::SetCameraControlMode(bool enabled) {
    cameraControlEnabled = enabled;

    if (enabled) {
        GetWindow()->SetCursorMode(GLFW_CURSOR_DISABLED);
        Logger::Log(LogLevel::INFO, "Camera control: ON");
    } else {
        GetWindow()->SetCursorMode(GLFW_CURSOR_NORMAL);
        Logger::Log(LogLevel::INFO, "Camera control: OFF (UI mode)");
    }
}

void Engine::OnInitialize() {
    Logger::Log(LogLevel::INFO, "Initializing WFE...");
    Logger::Log(LogLevel::INFO, "==================================");

    GetWindow()->SetFramebufferSizeCallback(FramebufferSizeCallback);
    GetWindow()->SetCursorPosCallback(MouseCallback);
    GetWindow()->SetScrollCallback(Input::ScrollCallback);
    GetWindow()->SetMouseButtonCallback(MouseButtonCallback);

    mm = GetModuleManager();

    mm->RegisterModule<ECSModule>();
    ecsModule = mm->GetModule<ECSModule>("ECS");
    ecsModule->Initialize();
    if (!ecsModule->IsInitialized()) {
        Logger::Log(LogLevel::WARNING,
                    "ECS failed to initialize");
    }

    mm->RegisterModule<ResourceModule>();
    resourceModule = mm->GetModule<ResourceModule>("Resource");
    resourceModule->Initialize();
    if (!resourceModule->IsInitialized()) {
        Logger::Log(LogLevel::WARNING,
                    "RedsourceModule failed to initialize");
    }

    mm->RegisterModule<RenderingModule>(
        GetWindow()->GetGLFWWindow(),
        ecsModule->GetECS(),
        mm);
    renderingModule = mm->GetModule<RenderingModule>("Rendering");
    renderingModule->Initialize();
    if (!renderingModule->IsInitialized()) {
        Logger::Log(LogLevel::WARNING,
                    "RenderingModule failed to initialize");
    }

    mm->RegisterModule<SceneModule>(ecsModule->GetECS());
    sceneModule = mm->GetModule<SceneModule>("Scene");
    sceneModule->Initialize();
    if (!sceneModule->IsInitialized()) {
        Logger::Log(LogLevel::WARNING,
                    "SceneModule failed to initialize");
    }

    mm->RegisterModule<PhysicsModule>(ecsModule->GetECS());
    m_physicsModule = mm->GetModule<PhysicsModule>("Physics");
    m_physicsModule->Initialize();
    if (!m_physicsModule->IsInitialized()) {
        Logger::Log(LogLevel::WARNING,
                    "PhysicsModule failed to initialize");
    }

    mm->RegisterModule<UIModule>(
        ecsModule->GetECS(),
        &mainCameraEntity,
        sceneModule->GetSceneManager(),
        mm,
        GetWindow()->GetGLFWWindow()
    );
    uiModule = mm->GetModule<UIModule>("UI");
    uiModule->Initialize();
    if (!uiModule->IsInitialized()) {
        Logger::Log(LogLevel::WARNING,
                    "UIModule failed to initialize");
    }

    scriptSystem = std::make_unique<ScriptSystem>();
    inputControllerSystem = std::make_unique<InputControllerSystem>();
    physicsDebugSystem = std::make_unique<PhysicsDebugRenderSystem>();
    audioSystem = std::make_unique<AudioSystem>();
    if (!audioSystem->Init()) {
        Logger::Log(LogLevel::WARNING, "AudioSystem failed to initialize");
    }

    InitializeAS();

    ech = std::make_unique<EditorCommandHandler>(mm);
    ech->RegisterAllCommands();
    RegistraterCoreCommands();

    auto editorCam = ecsModule->GetECS()->CreateCamera("Main Camera", true, true);
    SetMainCameraEntity(editorCam);
    SetCameraControlMode(false);

    CommandManager::ExecuteCommand("onDebugPauseToggle", {});

    renderingModule->GetRenderer()->SetEnableShadows(true);

    Logger::Log(LogLevel::INFO, "==================================");
    Logger::Log(LogLevel::INFO, "Engine initialized successfully");
}

void Engine::OnUpdate(float deltaTime) {
    mm->UpdateAll(deltaTime);

    ProcessInput();

    bool allowCameraControl = cameraControlEnabled && ShouldAllowCameraControl();
    inputControllerSystem->Update(
        *ecsModule->GetECS(),
        *GetInput(),
        deltaTime,
        allowCameraControl
    );

    UpdateMainCamera();

    scriptSystem->Update(*ecsModule->GetECS(), GetInput(), deltaTime);
    if (audioSystem)
        audioSystem->Update(ecsModule->GetECS());
}

void Engine::UpdateMainCamera() {
    auto *ecs = ecsModule->GetECS();
    entt::entity current = mainCameraEntity;

    if (current == entt::null || !ecs->IsValid(current) || !ecs->HasComponent<CameraComponent>(current)) {
        auto view = ecs->GetRegistry().view < CameraComponent > ();
        if (!view.empty()) {
            entt::entity found = entt::null;
            for (auto entity: view) {
                if (view.get<CameraComponent>(entity).isMainCamera) {
                    found = entity;
                    break;
                }
            }

            if (found == entt::null) found = view.front();

            SetMainCameraEntity(found);
        } else {
            SetMainCameraEntity(entt::null);
        }
    }
}

void Engine::OnRender() {
    auto *ecs = ecsModule->GetECS();
    auto *renderer = renderingModule->GetRenderer();

    entt::entity camera = ecs->FindGameCamera();
    if (camera == entt::null) {
        Logger::Log(LogLevel::WARNING, "No game camera found!");
        return;
    }

    renderer->BeginFrame();
    renderer->Render(
        *ecs,
        camera,
        GetWindow()->GetWidth(),
        GetWindow()->GetHeight()
    );
    renderer->EndFrame();

    if (showUI) {
        auto &transform = ecs->GetComponent<TransformComponent>(camera);
        auto &orientation = ecs->GetComponent<CameraOrientationComponent>(camera);
        auto &camComp = ecs->GetComponent<CameraComponent>(camera);

        glm::mat4 view = orientation.GetViewMatrix(transform.position);
        glm::mat4 projection = camComp.GetProjectionMatrix(
            (float) GetWindow()->GetWidth() / (float) GetWindow()->GetHeight()
        );

        physicsDebugSystem->Update(
            *ecs,
            *resourceModule->GetShaderManager(),
            "aabbDebug",
            view,
            projection
        );
    }

    if (showUI) {
        renderingModule->GetRenderer()->GetIcon()->Update(
            *ecs,
            *resourceModule->GetShaderManager(),
            "icon",
            ecs->GetComponent<CameraOrientationComponent>(camera).GetViewMatrix(
                ecs->GetComponent<TransformComponent>(camera).position),
            ecs->GetComponent<CameraComponent>(camera).GetProjectionMatrix(
                (float) GetWindow()->GetWidth() / (float) GetWindow()->GetHeight())
        );

        uiModule->GetImGuiManager()->BeginFrame();
        m_overlay.Render(ecs, mainCameraEntity, resourceModule->GetMaterialManager());
        uiModule->GetImGuiManager()->EndFrame();
    }
}

void Engine::OnShutdown() {
    Logger::Log(LogLevel::INFO, "Shutting down engine...");
    Logger::Log(LogLevel::INFO, "==================================");

    if (audioSystem)
        audioSystem->Shutdown();

    mm->ShutdownAll();

    Logger::Log(LogLevel::INFO, "==================================");
    Logger::Log(LogLevel::INFO, "Engine shutdown complete!");
}

bool Engine::ShouldAllowCameraControl() const {
    if (sceneModule && sceneModule->GetSceneManager())
        return !sceneModule->GetSceneManager()->IsInPlayMode();

    return true;
}

void Engine::SetMainCameraEntity(entt::entity newMainCameraEntity) {
    mainCameraEntity = newMainCameraEntity;
}

Engine::Engine(int w, int h, const std::string &title)
    : Application(w, h, title) {
}

void Engine::InitializeAS() {
    Logger::Log(LogLevel::INFO, "Initializing AngelScript...");

    try {
        InitAS(ecsModule->GetECS(), GetInput(), audioSystem.get());
        Logger::Log(LogLevel::INFO, "AngelScript initialized successfully");
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR, "Failed to initialize AngelScript: " + std::string(e.what()));
    }
}

void Engine::ProcessInput() {
    if (GetInput()->IsKeyJustPressed(Key::KEY_F5)) {
        resourceModule->GetShaderManager()->ReloadAll();
        Logger::Log(LogLevel::INFO, "Reloaded all shaders");
    }

    if (GetInput()->IsKeyJustPressed(Key::KEY_F1))
        showUI = !showUI;
}

void Engine::RegistraterCoreCommands() {
    CommandManager::RegisterCommand("onExit",
        [this](const CommandArgs &) {
            Logger::Log(LogLevel::INFO, "Exit requested from menu");
            Stop();
        });
}