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
        engine->m_coreModule->GetWindow()->SetSize(width, height);
}

void Engine::MouseCallback(GLFWwindow *window, const double xPos, const double yPos) {
    const auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    engine->m_uiModule->MouseCallback(window, xPos, yPos);
}

// void Engine::MouseButtonCallback(GLFWwindow *window, const int button, const int action, int /*mods*/) {
//     auto *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
//     if (!engine) return;
//
//     if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
//         if (engine->ShouldAllowCameraControl())
//             engine->SetCameraControlMode(!engine->cameraControlEnabled);
// }

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
    m_coreModule = GetCoreModule();

    const auto *window = m_coreModule->GetWindow();
    auto *glfwWin = window->GetGLFWWindow();

    glfwSetWindowUserPointer(glfwWin, this);

    window->SetFramebufferSizeCallback(FramebufferSizeCallback);
    window->SetCursorPosCallback(MouseCallback);
    window->SetScrollCallback(Input::ScrollCallback);

    m_ecsModule = InitModule<ECSModule>();
    if (!m_ecsModule) return false;

    m_resourceModule = InitModule<ResourceModule>();
    if (!m_resourceModule) return false;

    m_renderingModule = InitModule<RenderingModule>(glfwWin, m_ecsModule->GetECS(), mm);
    if (!m_renderingModule) return false;

    m_sceneModule = InitModule<SceneModule>(m_ecsModule->GetECS());
    if (!m_sceneModule) return false;

    m_physicsModule = InitModule<PhysicsModule>(m_ecsModule->GetECS());
    if (!m_physicsModule) return false;
    m_physicsModule->SetShouldUpdate(false);

    m_scriptModule = InitModule<ScriptModule>(mm);
    if (!m_scriptModule) return false;

    m_uiModule = InitModule<UIModule>(m_ecsModule->GetECS(), m_sceneModule->GetSceneManager(), mm, glfwWin);
    if (!m_uiModule) return false;

    // audioSystem = std::make_unique<AudioSystem>();
    // if (!audioSystem->Init()) {
    //     Logger::Log(LogLevel::CRITICAL, "AudioSystem failed to initialize");
    // }

    m_ech = std::make_unique<EditorCommandHandler>(mm);
    m_ech->RegisterAllCommands();
    RegistrateCoreCommands();
    PrefabsCommands::RegisterPrefabsCommands(m_ecsModule, m_resourceModule);

    gameCam = m_ecsModule->GetECS()->CreateCamera("Game Camera", true);
    m_ecsModule->GetECS()->AddComponent<c_Persistent>(gameCam);
    m_sceneModule->GetSceneManager()->SetPlayMode(false);

    CommandManager::ExecuteCommand("onDebugPauseToggle", {});

    m_renderingModule->GetRenderer()->SetEnableShadows(true);

    Logger::Log(LogLevel::INFO, "==================================");
    Logger::Log(LogLevel::INFO, "Engine initialized successfully");

    return true;
}

void Engine::OnSimulation(const float deltaTime) {
    m_physicsModule->Update(deltaTime);
    m_scriptModule->GetScriptSystem()->FixedUpdate(*m_ecsModule->GetECS(), deltaTime);
}

void Engine::OnUpdate(const float deltaTime) {
    ProcessInput();

    // if (audioSystem)
    //     audioSystem->Update(ecsModule->GetECS());

    mm->UpdateAll(deltaTime);
}

void Engine::OnRender() {
    auto *ecs = m_ecsModule->GetECS();
    auto *renderer = m_renderingModule->GetRenderer();
    auto *debugOverlay = m_uiModule->GetDebugOverlay();

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
    if (m_sceneModule &&m_sceneModule->GetSceneManager())
        return !m_sceneModule->GetSceneManager()->IsInPlayMode();

    return true;
}

Engine::Engine(const int w, const int h, const char* title)
    : Application(w, h, title) {
}

void Engine::ProcessInput() {
    const auto *input = m_coreModule->GetInput();

    if (input->IsKeyJustPressed(Key::KEY_F5)) {
        m_resourceModule->GetShaderManager()->ReloadAll();
        Logger::Log(LogLevel::INFO, "Reloaded all shaders");
    }
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

            m_coreModule->GetWindow()->SetVSync(*enabled);
            Logger::Log(LogLevel::INFO, std::string("VSync: ") + (*enabled ? "ON" : "OFF"));
        });
}