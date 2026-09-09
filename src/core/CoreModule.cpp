#include "CoreModule.h"

#include <GLFW/glfw3.h>

#include "core/logging/Logger.h"

CoreModule::CoreModule(int width, int height, const std::string &title) {
    m_window = std::make_unique<Window>(width, height, title);
    try {
        if (!m_window->Initialize()) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize Window");
            return;
        }
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR, e.what());

        return;
    }

    m_input = std::make_unique<Input>(m_window->GetGLFWWindow());
    m_time = std::make_unique<Time>();
    m_commandManager = std::make_unique<CommandManager>();
    m_eventBus = std::make_unique<EventBus>();
}

bool CoreModule::Initialize() { return true; }

void CoreModule::Update(float deltaTime) {
}

void CoreModule::Shutdown() {
}