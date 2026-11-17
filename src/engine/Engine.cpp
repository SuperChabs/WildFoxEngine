#include "Engine.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/glm.hpp>

#include "core/Input.h"
#include "core/CommandManager.h"
#include "core/logging/Logger.h"
#include "EngineCommandHandler.h"
#include "prefabs/PrefabsCommands.h"

void Engine::FramebufferSizeCallback(GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height);
    auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (engine)
        engine->coreModule->GetWindow()->SetSize(width, height);
}

void Engine::MouseCallback(GLFWwindow *window, const double xPos, const double yPos) {
    auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    Input *input = engine->coreModule->GetInput();
    input->UpdateMousePosition(xPos, yPos);

    if (ImGuizmo::IsUsing() || ImGui::GetIO().WantCaptureMouse)
        return;

    if (engine->cameraControlEnabled) {
        const glm::vec2 delta = input->GetMouseDelta();
        auto &orientation = engine->editorCam.orientation;
        const auto &config = engine->editorCam.camera;

        orientation.yaw += delta.x * config.mouseSensitivity;
        orientation.pitch += delta.y * config.mouseSensitivity;

        if (orientation.pitch > 89.0f) orientation.pitch = 89.0f;
        if (orientation.pitch < -89.0f) orientation.pitch = -89.0f;
    }
}

void Engine::MouseButtonCallback(GLFWwindow *window, const int button, const int action, int /*mods*/) {
    auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        if (engine->ShouldAllowCameraControl())
            engine->SetCameraControlMode(!engine->cameraControlEnabled);
}

void Engine::SetCameraControlMode(const bool enabled) {
    cameraControlEnabled = enabled;

    if (enabled) {
        coreModule->GetWindow()->SetCursorMode(GLFW_CURSOR_DISABLED);
        Logger::Log(LogLevel::INFO, "Camera control: ON");
    } else {
        coreModule->GetWindow()->SetCursorMode(GLFW_CURSOR_NORMAL);
        Logger::Log(LogLevel::INFO, "Camera control: OFF (UI mode)");
    }
}

template<typename T, typename... Args>
T *Engine::InitModule(Args &&... args) {
    T *module = mm->RegisterModule<T>(std::forward<Args>(args)...);
    if (module->Initialize())
        return module;

    if (module->IsRequired()) {
        Logger::Log(LogLevel::CRITICAL, std::string(module->GetName()) + " failed to initialize");
        return nullptr;
    }

    Logger::Log(LogLevel::WARNING, std::string("Optional module failed to initialize: ") + module->GetName());
    return module;
}

bool Engine::OnInitialize() {
    Logger::Log(LogLevel::INFO, "Initializing WFE...");
    Logger::Log(LogLevel::INFO, "==================================");

    mm = GetModuleManager();
    coreModule = GetCoreModule();

    const auto *window = coreModule->GetWindow();
    auto *glfwWin = window->GetGLFWWindow();

    glfwSetWindowUserPointer(glfwWin, this);

    window->SetFramebufferSizeCallback(FramebufferSizeCallback);
    window->SetCursorPosCallback(MouseCallback);
    window->SetScrollCallback(Input::ScrollCallback);
    window->SetMouseButtonCallback(MouseButtonCallback);

    ecsModule = InitModule<ECSModule>();
    if (!ecsModule) return false;

    resourceModule = InitModule<ResourceModule>();
    if (!resourceModule) return false;

    renderingModule = InitModule<RenderingModule>(glfwWin, ecsModule->GetECS(), mm);
    if (!renderingModule) return false;

    sceneModule = InitModule<SceneModule>(ecsModule->GetECS());
    if (!sceneModule) return false;

    m_physicsModule = InitModule<PhysicsModule>(ecsModule->GetECS());
    if (!m_physicsModule) return false;
    m_physicsModule->SetShouldUpdate(false); // physics runs at a fixed step from OnSimulation()

    m_scriptModule = InitModule<ScriptModule>(mm);
    if (!m_scriptModule) return false;

    uiModule = InitModule<UIModule>(ecsModule->GetECS(), sceneModule->GetSceneManager(), mm, glfwWin);
    if (!uiModule) return false;

    // audioSystem = std::make_unique<AudioSystem>();
    // if (!audioSystem->Init()) {
    //     Logger::Log(LogLevel::CRITICAL, "AudioSystem failed to initialize");
    // }

    inputControllerSystem = std::make_unique<InputControllerSystem>();
    physicsDebugSystem = std::make_unique<PhysicsDebugRenderSystem>();

    m_ech = std::make_unique<EditorCommandHandler>(mm);
    m_ech->RegisterAllCommands();
    RegistrateCoreCommands();
    PrefabsCommands::RegisterPrefabsCommands(ecsModule, resourceModule);

    gameCam = ecsModule->GetECS()->CreateCamera("Game Camera", true);
    ecsModule->GetECS()->AddComponent<c_Persistent>(gameCam);
    SetCameraControlMode(false);
    sceneModule->GetSceneManager()->SetPlayMode(false);

    CommandManager::ExecuteCommand("onDebugPauseToggle", {});

    renderingModule->GetRenderer()->SetEnableShadows(true);

    Logger::Log(LogLevel::INFO, "==================================");
    Logger::Log(LogLevel::INFO, "Engine initialized successfully");

    return true;
}

void Engine::OnSimulation(const float deltaTime) {
    m_physicsModule->Update(deltaTime);
}

void Engine::OnUpdate(const float deltaTime) {
    ProcessInput();

    bool allowCameraControl = cameraControlEnabled && ShouldAllowCameraControl();
    InputControllerSystem::Update(
        editorCam.camera,
        editorCam.transform,
        editorCam.orientation,
        *coreModule->GetInput(),
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
    auto *debugOverlay = uiModule->GetDebugOverlay();

    Framebuffer *sceneFB = debugOverlay->GetEditorFramebuffer();
    const ImVec2 sceneViewportSize = debugOverlay->GetEditorViewportSize();
    if (debugOverlay->GetEditViewportWindow()->IsOpen()) {
        sceneFB->Bind();

        renderer->BeginFrame();
        renderer->Render(
            editorCam.camera,
            editorCam.transform,
            editorCam.orientation,
            static_cast<int>(sceneViewportSize.x),
            static_cast<int>(sceneViewportSize.y)
        );
        renderer->EndFrame();

        const glm::mat4 view = editorCam.orientation.GetViewMatrix(editorCam.transform.position);
        const glm::mat4 projection = editorCam.camera.GetProjectionMatrix(
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
            editorCam.orientation.GetViewMatrix(editorCam.transform.position)
        );

        sceneFB->Unbind();
    }

    Framebuffer *gameFB = debugOverlay->GetGameFramebuffer();
    const ImVec2 gameViewportSize = debugOverlay->GetGameViewportSize();
    if (debugOverlay->GetGameViewportWindow()->IsOpen()) {
        gameFB->Bind();

        renderer->BeginFrame();
        renderer->Render(
            ecs->GetComponent<c_Camera>(gameCam),
            ecs->GetComponent<c_Transform>(gameCam),
            ecs->GetComponent<c_CameraOrientation>(gameCam),
            static_cast<int>(gameViewportSize.x),
            static_cast<int>(gameViewportSize.y)
        );
        renderer->EndFrame();

        gameFB->Unbind();
    }

    uiModule->GetImGuiManager()->BeginFrame(cameraControlEnabled);
    uiModule->RenderUI(editorCam, physicsDebugSystem.get());
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
    if (sceneModule &&sceneModule->GetSceneManager())
        return !sceneModule->GetSceneManager()->IsInPlayMode();

    return true;
}

Engine::Engine(const int w, const int h, char* &title)
    : Application(w, h, title) {
}

void Engine::ProcessInput() {
    const auto *input = coreModule->GetInput();

    if (input->IsKeyJustPressed(Key::KEY_F5)) {
        resourceModule->GetShaderManager()->ReloadAll();
        Logger::Log(LogLevel::INFO, "Reloaded all shaders");
    }

    if (input->IsKeyJustPressed(Key::KEY_F1))
        showUI = !showUI;
}

void Engine::RegistrateCoreCommands() {
    CommandManager::RegisterCommand("onExit",
        [this](const CommandArgs &) {
            Logger::Log(LogLevel::INFO, "Exit requested from menu");
            Stop();
        });

    CommandManager::RegisterCommand("onSetVSync",
        [this](const CommandArgs &args) {
            const bool *enabled = GetArg<bool>(args, 0, "onSetVSync");
            if (!enabled) return;

            coreModule->GetWindow()->SetVSync(*enabled);
            Logger::Log(LogLevel::INFO, std::string("VSync: ") + (*enabled ? "ON" : "OFF"));
        });
}