#include "Application.h"
#include "core/logging/Logger.h"

Application::Application(int width, int height, const std::string &title)
    : isRunning(false) {
    moduleManager = std::make_unique<ModuleManager>();

    m_coreModule = moduleManager->RegisterModule<CoreModule>(width, height, title);
    m_coreModule->Initialize();
    if (!m_coreModule->IsInitialized()) Logger::Log(LogLevel::CRITICAL, "Core failed to initialize");
}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    Logger::AddSink(&console);
    // Logger::AddSink(&file);

    OnInitialize();

    Logger::Log(LogLevel::INFO, "Application initialized successfully");

    return true;
}

void Application::Run() {
    isRunning = true;

    while (isRunning && !m_coreModule->GetWindow()->ShouldClose()) {
        m_coreModule->GetTime()->Update();

        Update();
        OnRender();

        m_coreModule->GetWindow()->SwapBuffers();
        m_coreModule->GetWindow()->PollEvents();
    }
}

void Application::Shutdown() {
    OnShutdown();

    Logger::RemoveSink(&console);
    // Logger::RemoveSink(&file);

    Logger::Log(LogLevel::INFO, "Application shutdown complete");
}

void Application::Stop() {
    isRunning = false;
}

ModuleManager *Application::GetModuleManager() {
    return moduleManager.get();
}

void Application::Update() {
    float deltaTime = m_coreModule->GetTime()->GetDeltaTime();

    OnUpdate(deltaTime);
}