#include "Engine.h"

#include <imgui.h>

#include <ImGuizmo.h>
#include <glm/glm.hpp>
#include "core/Input.h"
#include "core/CommandManager.h"
#include "core/logging/Logger.h"
#include "EngineCommandHandler.h"

void Engine::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (app && app->GetModuleManager()->GetModule<CoreModule>("Core")->GetWindow())
        app->GetModuleManager()->GetModule<CoreModule>("Core")->GetWindow()->SetSize(width, height);
}

void Engine::MouseCallback(GLFWwindow *window, double xpos, double ypos) {
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    engine->mm->GetModule<CoreModule>("Core")->GetInput()->UpdateMousePosition(xpos, ypos);

    if (ImGuizmo::IsUsing() || ImGui::GetIO().WantCaptureMouse)
        return;

    if (engine->cameraControlEnabled) {
        glm::vec2 delta = engine->mm->GetModule<CoreModule>("Core")->GetInput()->GetMouseDelta();
        auto &orientation = engine->editorCam.orientation;
        auto &config = engine->editorCam.camera;

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
        mm->GetModule<CoreModule>("Core")->GetWindow()->SetCursorMode(GLFW_CURSOR_DISABLED);
        Logger::Log(LogLevel::INFO, "Camera control: ON");
    } else {
        mm->GetModule<CoreModule>("Core")->GetWindow()->SetCursorMode(GLFW_CURSOR_NORMAL);
        Logger::Log(LogLevel::INFO, "Camera control: OFF (UI mode)");
    }
}

void Engine::OnInitialize() {
    Logger::Log(LogLevel::INFO, "Initializing WFE...");
    Logger::Log(LogLevel::INFO, "==================================");

    mm = GetModuleManager();

    auto *core = mm->GetModule<CoreModule>("Core");
    auto *window = core->GetWindow();
    auto *glfwWin = window->GetGLFWWindow();

    glfwSetWindowUserPointer(glfwWin, this);

    mm->GetModule<CoreModule>("Core")->GetWindow()->SetFramebufferSizeCallback(FramebufferSizeCallback);
    mm->GetModule<CoreModule>("Core")->GetWindow()->SetCursorPosCallback(MouseCallback);
    mm->GetModule<CoreModule>("Core")->GetWindow()->SetScrollCallback(Input::ScrollCallback);
    mm->GetModule<CoreModule>("Core")->GetWindow()->SetMouseButtonCallback(MouseButtonCallback);

    mm->RegisterModule<ECSModule>();
    ecsModule = mm->GetModule<ECSModule>("ECS");
    ecsModule->Initialize();
    if (!ecsModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "ECS failed to initialize");

    mm->RegisterModule<ResourceModule>();
    resourceModule = mm->GetModule<ResourceModule>("Resource");
    resourceModule->Initialize();
    if (!resourceModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "RedsourceModule failed to initialize");

    mm->RegisterModule<RenderingModule>(mm->GetModule<CoreModule>("Core")->GetWindow()->GetGLFWWindow(),
        ecsModule->GetECS(), mm);
    renderingModule = mm->GetModule<RenderingModule>("Rendering");
    renderingModule->Initialize();
    if (!renderingModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "RenderingModule failed to initialize");

    mm->RegisterModule<SceneModule>(ecsModule->GetECS());
    sceneModule = mm->GetModule<SceneModule>("Scene");
    sceneModule->Initialize();
    if (!sceneModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "SceneModule failed to initialize");

    mm->RegisterModule<PhysicsModule>(ecsModule->GetECS());
    m_physicsModule = mm->GetModule<PhysicsModule>("Physics");
    m_physicsModule->Initialize();
    if (!m_physicsModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "PhysicsModule failed to initialize");

    m_scriptModule = mm->RegisterModule<ScriptModule>(mm);
    m_scriptModule->Initialize();
    if (!m_scriptModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "ScriptModule failed to initialize");

    mm->RegisterModule<UIModule>(ecsModule->GetECS(), sceneModule->GetSceneManager(), mm,
        mm->GetModule<CoreModule>("Core")->GetWindow()->GetGLFWWindow()
    );
    uiModule = mm->GetModule<UIModule>("UI");
    uiModule->Initialize();
    if (!uiModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "UIModule failed to initialize");

    inputControllerSystem = std::make_unique<InputControllerSystem>();
    physicsDebugSystem = std::make_unique<PhysicsDebugRenderSystem>();

    // audioSystem = std::make_unique<AudioSystem>();
    // if (!audioSystem->Init()) {
    //     Logger::Log(LogLevel::CRITICAL, "AudioSystem failed to initialize");
    // }

    m_ech = std::make_unique<EditorCommandHandler>(mm);
    m_ech->RegisterAllCommands();
    RegistraterCoreCommands();

    gameCam = ecsModule->GetECS()->CreateCamera("Main Camera", true);
    SetCameraControlMode(false);
    sceneModule->GetSceneManager()->SetPlayMode(false);

    CommandManager::ExecuteCommand("onDebugPauseToggle", {});

    renderingModule->GetRenderer()->SetEnableShadows(true);

    Logger::Log(LogLevel::INFO, "==================================");
    Logger::Log(LogLevel::INFO, "Engine initialized successfully");
}

void Engine::OnUpdate(float deltaTime) {
    ProcessInput();

    bool allowCameraControl = cameraControlEnabled && ShouldAllowCameraControl();
    inputControllerSystem->Update(
        editorCam.camera,
        editorCam.transform,
        editorCam.orientation,
        *mm->GetModule<CoreModule>("Core")->GetInput(),
        deltaTime,
        allowCameraControl
    );

    // if (audioSystem)
    //     audioSystem->Update(ecsModule->GetECS());

    mm->UpdateAll(deltaTime);
}

void Engine::OnRender() {
    auto *ecs = ecsModule->GetECS();
    auto *renderer = renderingModule->GetRenderer();

    CameraComponent camera;
    TransformComponent transform;
    CameraOrientationComponent orientation;

    if (sceneModule->GetSceneManager()->IsInPlayMode()) {
        camera = ecs->GetComponent<CameraComponent>(gameCam);
        transform = ecs->GetComponent<TransformComponent>(gameCam);
        orientation = ecs->GetComponent<CameraOrientationComponent>(gameCam);
    } else if (!sceneModule->GetSceneManager()->IsInPlayMode()) {
        camera = editorCam.camera;
        transform = editorCam.transform;
        orientation = editorCam.orientation;
    } else {
        Logger::Log(LogLevel::CRITICAL, "No camera found");
        return;
    }

    Framebuffer* sceneFB = uiModule->GetDebugOverlay()->GetFramebuffer();
    ImVec2 sceneViewportSize = uiModule->GetDebugOverlay()->GetViewportSize();
    if (sceneViewportSize.x <= 0 || sceneViewportSize.y <= 0)
    {
        Logger::Log(LogLevel::WARNING, "Invalid scene viewport size, skipping render");
    }
    else
    {
        sceneFB->Bind();

        renderer->BeginFrame();
        renderer->Render(
            editorCam.camera,
            editorCam.transform,
            editorCam.orientation,
            sceneViewportSize.x,
            sceneViewportSize.y
        );
        renderer->EndFrame();

        glm::mat4 view = orientation.GetViewMatrix(transform.position);
        glm::mat4 projection = camera.GetProjectionMatrix(
        sceneViewportSize.x / sceneViewportSize.y
        );

        physicsDebugSystem->Update(
            *ecs,
            *resourceModule->GetShaderManager(),
            "aabbDebug",
            view,
            projection
        );

        renderingModule->GetRenderer()->GetIcon()->Update(
            *ecs,
            *resourceModule->GetShaderManager(),
            "icon",
            orientation.GetViewMatrix(transform.position),
            projection
        );

        sceneFB->Unbind();
    }

    uiModule->GetImGuiManager()->BeginFrame(cameraControlEnabled);
    uiModule->RenderUI(editorCam);
    uiModule->GetImGuiManager()->EndFrame();
}

void Engine::OnShutdown() {
    Logger::Log(LogLevel::INFO, "Shutting down engine...");
    Logger::Log(LogLevel::INFO, "==================================");

    // if (audioSystem)
    //     audioSystem->Shutdown();

    mm->ShutdownAll();

    Logger::Log(LogLevel::INFO, "==================================");
    Logger::Log(LogLevel::INFO, "Engine shutdown complete!");
}

bool Engine::ShouldAllowCameraControl() const {
    if (sceneModule && sceneModule->GetSceneManager())
        return !sceneModule->GetSceneManager()->IsInPlayMode();

    return true;
}

Engine::Engine(int w, int h, const std::string &title)
    : Application(w, h, title) {
}

void Engine::ProcessInput() {
    if (mm->GetModule<CoreModule>("Core")->GetInput()->IsKeyJustPressed(Key::KEY_F5)) {
        resourceModule->GetShaderManager()->ReloadAll();
        Logger::Log(LogLevel::INFO, "Reloaded all shaders");
    }

    if (mm->GetModule<CoreModule>("Core")->GetInput()->IsKeyJustPressed(Key::KEY_F1))
        showUI = !showUI;
}

void Engine::RegistraterCoreCommands() {
    CommandManager::RegisterCommand("onExit",
        [this](const CommandArgs &) {
            Logger::Log(LogLevel::INFO, "Exit requested from menu");
            Stop();
        });
}