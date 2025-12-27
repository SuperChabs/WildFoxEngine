#include "core/Window.h"
#include <iostream>

Window* Window::instance = nullptr;

Window::Window(int w, int h, const std::string& t)
    : window(nullptr), width(w), height(h), title(t)
{
    instance = this;
}

Window::~Window()
{
    Terminate();
}

bool Window::Initialize()
{
    // Don't force a specific platform; only hint X11 on Linux builds
#if defined(__linux__)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
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
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    
    std::cout << "Successful created GLFW window\n";
    
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }
    
    std::cout << "Successful initialize GLAD\n";
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    
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