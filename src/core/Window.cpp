#include "Window.h"

#include <utility>
#include "core/logging/Logger.h"

Window::Window(const int width, const int height, std::string title)
    : window(nullptr), width(width), height(height), title(std::move(title)) {
}

Window::~Window() {
    Terminate();
}

bool Window::Initialize() {
#if defined(__linux__)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    if (!glfwInit()) {
        Logger::Log(LogLevel::ERROR, "Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        Logger::Log(LogLevel::ERROR, "Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    Logger::Log(LogLevel::INFO,
                "GLFW window created: " + title + " (" + std::to_string(width) + "x" + std::to_string(height) +
                ")");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        Logger::Log(LogLevel::ERROR, "Failed to initialize GLAD");
        return false;
    }

    Logger::Log(LogLevel::INFO, "GLAD initialized successfully");
    Logger::Log(LogLevel::INFO,
                "OpenGL version: " + std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION))));

    return true;
}

void Window::Terminate() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
        Logger::Log(LogLevel::INFO, "Window destroyed: " + title);
    }

    glfwTerminate();
    Logger::Log(LogLevel::INFO, "GLFW terminated");
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(window);
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::SetFramebufferSizeCallback(const GLFWframebuffersizefun callback) const {
    glfwSetFramebufferSizeCallback(window, callback);
}

void Window::SetCursorPosCallback(const GLFWcursorposfun callback) const {
    glfwSetCursorPosCallback(window, callback);
}

void Window::SetScrollCallback(const GLFWscrollfun callback) const {
    glfwSetScrollCallback(window, callback);
}

void Window::SetMouseButtonCallback(const GLFWmousebuttonfun callback) const {
    glfwSetMouseButtonCallback(window, callback);
}

void Window::SetCursorMode(const int mode) const {
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}

void Window::SetSize(const int w, const int h) {
    width = w;
    height = h;
}

GLFWwindow *Window::GetGLFWWindow() const {
    return window;
}

int Window::GetWidth() const {
    return width;
}

int Window::GetHeight() const {
    return height;
}

float Window::GetAspectRatio() const {
    return static_cast<float>(width) / static_cast<float>(height);
}
