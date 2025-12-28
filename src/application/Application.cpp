#include <glad/glad.h>

#include <glm/glm.hpp>

#include "application/Application.h"
#include "utils/Logger.h"

// крч хз, ну тіпа не клас
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Application::Application(int width, int height, const std::string& title)
        : window(nullptr), input(nullptr), time(nullptr), camera(nullptr),
            renderer(nullptr), textureManager(nullptr), sceneManager(nullptr),
            isRunning(false), showUI(true), cameraControlEnabled(false)
{
    window = std::make_unique<Window>(width, height, title);

    Logger::Log(LogLevel::INFO, "Application created");
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!window->Initialize())
    {
        Logger::Log(LogLevel::ERROR, "Failed to initialize Window");
        return false;
    }

    glfwSetWindowUserPointer(window->GetGLFWWindow(), this);
    
    window->SetFramebufferSizeCallback(FramebufferSizeCallback);
    window->SetCursorPosCallback(MouseCallback);
    window->SetScrollCallback(Input::ScrollCallback);
    window->SetMouseButtonCallback(MouseButtonCallback);
    
    input = std::make_unique<Input>(window->GetGLFWWindow());
    time = std::make_unique<Time>();
    camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    renderer = std::make_unique<Renderer>();
    textureManager = std::make_unique<TextureManager>();
    sceneManager = std::make_unique<SceneManager>();
    imGuiManager = std::make_unique<ImGuiManager>();
    
    renderer->Initialize();
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    imGuiManager = std::make_unique<ImGuiManager>();
    if (!imGuiManager->Initialize(window->GetGLFWWindow())) 
        return false;

    SetCameraControlMode(false);    
    
    Logger::Log(LogLevel::INFO, "Application initialized successfully");
    
    OnInitialize();
    
    return true;
}

void Application::Run()
{
    isRunning = true;
    
    while (isRunning && !window->ShouldClose())
    {
        time->Update();
        
        ProcessInput();
        Update();
        Render();
        
        window->SwapBuffers();
        window->PollEvents();
    }
}

void Application::ProcessInput()
{
    if (input->IsKeyPressed(XKey::KEY_ESCAPE)) 
        Stop();
    
    if (input->IsKeyJustPressed(XKey::KEY_F1)) 
        showUI = !showUI;
    
    if (cameraControlEnabled) 
    {
        if (input->IsKeyPressed(XKey::KEY_W)) 
            camera->ProcessKeyboard(FORWARD, time->GetDeltaTime());
        if (input->IsKeyPressed(XKey::KEY_S)) 
            camera->ProcessKeyboard(BACKWARD, time->GetDeltaTime()); 
        if (input->IsKeyPressed(XKey::KEY_A)) 
            camera->ProcessKeyboard(LEFT, time->GetDeltaTime());
        if (input->IsKeyPressed(XKey::KEY_D)) 
            camera->ProcessKeyboard(RIGHT, time->GetDeltaTime());

        if (input->IsKeyPressed(XKey::KEY_SPACE)) 
            camera->ProcessKeyboard(UP, time->GetDeltaTime());
        if (input->IsKeyPressed(XKey::KEY_LEFT_SHIFT)) 
            camera->ProcessKeyboard(DOWN, time->GetDeltaTime());
    }
}

void Application::Update()
{
    float deltaTime = time->GetDeltaTime();
    sceneManager->Update(deltaTime);
    
    OnUpdate(deltaTime);
}

void Application::Render()
{
    renderer->BeginFrame();
    
    OnRender();

    if (showUI) 
    {
        imGuiManager->BeginFrame();
        RenderUI();
        imGuiManager->EndFrame();
    }    
    
    renderer->EndFrame();
}

void Application::RenderUI() 
{
}

void Application::Shutdown()
{
    OnShutdown();
    
    Logger::Log(LogLevel::INFO, "Application shutdown complete");
}

void Application::SetCameraControlMode(bool enabled) 
{
    cameraControlEnabled = enabled;
    
    if (enabled) 
    {
        window->SetCursorMode(GLFW_CURSOR_DISABLED);
        Logger::Log(LogLevel::INFO, "Camera control: ON");
    } else 
    {
        window->SetCursorMode(GLFW_CURSOR_NORMAL);
        Logger::Log(LogLevel::INFO, "Camera control: OFF (UI mode)");
    }
}

void Application::MouseCallback(GLFWwindow* window, double xpos, double ypos) 
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (!app || !app->input || !app->camera)
        return;
    
    app->input->UpdateMousePosition(xpos, ypos);
    
    if (app->cameraControlEnabled) 
    {
        glm::vec2 delta = app->input->GetMouseDelta();
        app->camera->ProcessMouseMovement(delta.x, delta.y);
    }
}

void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) 
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (!app || !app->input || !app->camera)
        return;
    
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) 
        app->SetCameraControlMode(!app->cameraControlEnabled);
}