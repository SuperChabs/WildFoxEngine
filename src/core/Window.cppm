module;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

export module WFE.Core.Window;

import WFE.Core.Logger;

/// @file Window.cppm
/// @brief Window class handler
/// @author SuperChabs
/// @date 2026-01-28

export class Window 
{
private:
    GLFWwindow* window;
    int width;
    int height;
    std::string title;

public:
    Window(int width, int height, const std::string& title)
        : window(nullptr), width(width), height(height), title(title) {}
    ~Window()
    {
        Terminate();
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool Initialize()
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

    void Terminate()
    {
        if (window)
        {
            glfwDestroyWindow(window);
            window = nullptr;
            Logger::Log(LogLevel::INFO, "Window destroyed: " + title);
        }

        glfwTerminate();
        Logger::Log(LogLevel::INFO, "GLFW terminated");
    }
    
    bool ShouldClose() const
    {
        return glfwWindowShouldClose(window);
    }

    void SwapBuffers()
    {
        glfwSwapBuffers(window);
    }

    void PollEvents()
    {
        glfwPollEvents();
    }
    
    void SetFramebufferSizeCallback(GLFWframebuffersizefun callback)
    {
        glfwSetFramebufferSizeCallback(window, callback);
    }

    void SetCursorPosCallback(GLFWcursorposfun callback)
    {
        glfwSetCursorPosCallback(window, callback);
    }

    void SetScrollCallback(GLFWscrollfun callback)
    {
        glfwSetScrollCallback(window, callback);
    }

    void SetMouseButtonCallback(GLFWmousebuttonfun callback)
    {
        glfwSetMouseButtonCallback(window, callback);
    }

    void SetCursorMode(int mode)
    {
        glfwSetInputMode(window, GLFW_CURSOR, mode);
    }
    
    void SetSize(int w, int h) { width = w;height = h; }

    GLFWwindow* GetGLFWWindow() { return window; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    float GetAspectRatio() const { return static_cast<float>(width) / static_cast<float>(height); }
    
};