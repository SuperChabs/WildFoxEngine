#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>

class Window 
{
private:
    GLFWwindow* window;
    int width;
    int height;
    std::string title;

public:
    Window(int width, int height, const std::string& title);    
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool Initialize();
    void Terminate();
    
    bool ShouldClose() const;
    void SwapBuffers();
    void PollEvents();
    
    void SetFramebufferSizeCallback(GLFWframebuffersizefun callback);
    void SetCursorPosCallback(GLFWcursorposfun callback);
    void SetScrollCallback(GLFWscrollfun callback);
    void SetMouseButtonCallback(GLFWmousebuttonfun callback);
    
    void SetCursorMode(int mode);
    
    GLFWwindow* GetGLFWWindow() { return window; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    float GetAspectRatio() const { return static_cast<float>(width) / static_cast<float>(height); }
    
    // static Window& GetInstance(int width, int height, const std::string& title) 
    // {
    //     static Window instance(width, height, title);
    //     return instance;
    // }
};

#endif