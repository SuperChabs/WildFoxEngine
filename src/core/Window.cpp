#include "core/Window.h"
#include "utils/Logger.h"
#include <iostream>

Window::Window(int w, int h, const std::string& t)
    : window(nullptr), width(w), height(h), title(t) {}

Window::~Window()
{
    Terminate();
}

bool Window::Initialize()
{
#if defined(__linux__)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    if (!glfwInit())
    {
        Logger::Log(LogLevel::ERROR, "Failed to initialize GLFW");
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
        Logger::Log(LogLevel::ERROR, "Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    
    Logger::Log(LogLevel::INFO, "GLFW window created: " + title + " (" + std::to_string(width) + "x" + std::to_string(height) + ")");
    
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger::Log(LogLevel::ERROR, "Failed to initialize GLAD");
        return false;
    }
    
    Logger::Log(LogLevel::INFO, "GLAD initialized successfully");
    Logger::Log(LogLevel::INFO, "OpenGL version: " + std::string((const char*)glGetString(GL_VERSION)));
    
    return true;
}

void Window::Terminate()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();

    Logger::Log(LogLevel::INFO, "GLFW terminated");
    Logger::Log(LogLevel::INFO, "Window destroyed: " + title);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(window);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(window);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SetFramebufferSizeCallback(GLFWframebuffersizefun callback)
{
    glfwSetFramebufferSizeCallback(window, callback);
}

void Window::SetCursorPosCallback(GLFWcursorposfun callback)
{
    glfwSetCursorPosCallback(window, callback);
}

void Window::SetScrollCallback(GLFWscrollfun callback)
{
    glfwSetScrollCallback(window, callback);
}

void Window::SetMouseButtonCallback(GLFWmousebuttonfun callback)
{
    glfwSetMouseButtonCallback(window, callback);
}

void Window::SetCursorMode(int mode)
{
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}