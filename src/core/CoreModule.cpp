#include "CoreModule.h"

#include <utility>

#include <GLFW/glfw3.h>

#include "core/logging/Logger.h"

CoreModule::CoreModule(const int width, const int height, char* title)
    : m_width(width), m_height(height), m_title(title) {
}

bool CoreModule::OnInitialize() {
    m_window = std::make_unique<Window>(m_width, m_height, m_title);
    try {
        if (!m_window->Initialize()) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize Window");
            return false;
        }
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR, e.what());
        return false;
    }

    m_input = std::make_unique<Input>(m_window->GetGLFWWindow());
    m_time = std::make_unique<Time>();

    return true;
}

void CoreModule::Update(float deltaTime) {
}

void CoreModule::Shutdown() {
}
