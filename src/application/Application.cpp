#include "application/Application.h"
#include <glad/glad.h>
#include <iostream>

Application* Application::instance = nullptr;

Application::Application(int width, int height, const std::string& title)
    : window(nullptr), input(nullptr), time(nullptr), camera(nullptr),
      renderer(nullptr), textureManager(nullptr), sceneManager(nullptr),
      isRunning(false)
{
    instance = this;
    window = new Window(width, height, title);
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!window->Initialize())
    {
        std::cerr << "Failed to initialize window\n";
        return false;
    }
    
    window->SetFramebufferSizeCallback(FramebufferSizeCallback);
    window->SetCursorPosCallback(MouseCallback);
    window->SetScrollCallback(Input::ScrollCallback);
    window->SetCursorMode(GLFW_CURSOR_DISABLED);
    
    input = new Input(window->GetGLFWWindow());
    time = new Time();
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    renderer = new Renderer();
    textureManager = new TextureManager();
    sceneManager = new SceneManager();
    
    renderer->Initialize();
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    
    std::cout << "Application initialized successfully\n";
    
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
    if (input->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        isRunning = false;
    }
    
    float deltaTime = time->GetDeltaTime();
    
    if (input->IsKeyPressed(GLFW_KEY_W))
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (input->IsKeyPressed(GLFW_KEY_S))
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (input->IsKeyPressed(GLFW_KEY_A))
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (input->IsKeyPressed(GLFW_KEY_D))
        camera->ProcessKeyboard(RIGHT, deltaTime);
    if (input->IsKeyPressed(GLFW_KEY_SPACE))
        camera->ProcessKeyboard(UP, deltaTime);
    if (input->IsKeyPressed(GLFW_KEY_LEFT_ALT))
        camera->ProcessKeyboard(DOWN, deltaTime);
    
    float scrollOffset = input->GetScrollOffset();
    if (scrollOffset != 0.0f)
    {
        camera->ProcessScrool(scrollOffset);
        input->ResetScrollOffset();
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
    
    renderer->EndFrame();
}

void Application::Shutdown()
{
    OnShutdown();
    
    delete sceneManager;
    delete textureManager;
    delete renderer;
    delete camera;
    delete time;
    delete input;
    delete window;
    
    sceneManager = nullptr;
    textureManager = nullptr;
    renderer = nullptr;
    camera = nullptr;
    time = nullptr;
    input = nullptr;
    window = nullptr;
    
    std::cout << "Application shutdown complete\n";
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Application::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (!instance || !instance->input || !instance->camera)
        return;
    
    instance->input->UpdateMousePosition(xpos, ypos);
    glm::vec2 delta = instance->input->GetMouseDelta();
    instance->camera->ProcessMouseMovement(delta.x, delta.y);
}