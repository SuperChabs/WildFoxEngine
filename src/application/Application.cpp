#include "Application.h"
#include "core/logging/Logger.h"

void Application::Update() {
    float deltaTime = time->GetDeltaTime();

    OnUpdate(deltaTime);
}

Application::Application(int width, int height, const std::string &title)
    : window(nullptr), input(nullptr), time(nullptr),
      isRunning(false) {
    window = std::make_unique<Window>(width, height, title);

    Logger::Log(LogLevel::INFO, "Application created");
}


Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    if (!window->Initialize()) {
        Logger::Log(LogLevel::ERROR, "Failed to initialize Window");
        return false;
    }

    glfwSetWindowUserPointer(window->GetGLFWWindow(), this);

    input = std::make_unique<Input>(window->GetGLFWWindow());
    time = std::make_unique<Time>();

    moduleManager = std::make_unique<ModuleManager>();

    Logger::AddSink(&console);
    Logger::AddSink(&file);

    OnInitialize();

    Logger::Log(LogLevel::INFO, "Application initialized successfully");

    return true;
}

void Application::Run() {
    isRunning = true;

    while (isRunning && !window->ShouldClose()) {
        time->Update();

        Update();
        OnRender();

        window->SwapBuffers();
        window->PollEvents();
    }
}

void Application::Shutdown() {
    OnShutdown();

    Logger::RemoveSink(&console);
    Logger::RemoveSink(&file);

    Logger::Log(LogLevel::INFO, "Application shutdown complete");
}

void Application::Stop() {
    isRunning = false;
}

Window *Application::GetWindow() {
    return window.get();
}

Input *Application::GetInput() {
    return input.get();
}

Time *Application::GetTime() const {
    return time.get();
}

ModuleManager *Application::GetModuleManager() {
    return moduleManager.get();
}