#include "Application.h"

#include "core/logging/Logger.h"

Application::Application(int width, int height, const char* title)
    : m_isRunning(false) {
    moduleManager = std::make_unique<ModuleManager>();

    m_coreModule = moduleManager->RegisterModule<CoreModule>(width, height, title);
    if (!m_coreModule->Initialize())
        Logger::Log(LogLevel::CRITICAL, "Core failed to initialize");
}

bool Application::Initialize() {
    Logger::AddSink(&m_console);
    // Logger::AddSink(&file);

    if (!m_coreModule->IsInitialized() || !OnInitialize()) {
        Logger::Log(LogLevel::CRITICAL, "Application failed to initialize");
        return false;
    }

    Logger::Log(LogLevel::INFO, "Application initialized successfully");

    return true;
}

void Application::Run() {
    m_isRunning = true;

    while (!m_coreModule->GetWindow()->ShouldClose()) {
        if (!m_isRunning) break;

        m_coreModule->GetTime()->Update();

        const float deltaTime = m_coreModule->GetTime()->GetDeltaTime();

        m_accumulator += deltaTime;
        m_accumulator = std::min(m_accumulator, 1.0f);

        while (m_accumulator >= FIXED_DT) {
            OnSimulation(FIXED_DT);
            m_accumulator -= FIXED_DT;
        }

        OnUpdate(deltaTime);
        OnRender();

        m_coreModule->GetWindow()->SwapBuffers();
        m_coreModule->GetWindow()->PollEvents();
    }
}

void Application::Shutdown() {
    OnShutdown();

    Logger::RemoveSink(&m_console);
    // Logger::RemoveSink(&file);

    Logger::Log(LogLevel::INFO, "Application shutdown complete");
}

void Application::Stop() {
    m_isRunning = false;
}

ModuleManager *Application::GetModuleManager() const {
    return moduleManager.get();
}
