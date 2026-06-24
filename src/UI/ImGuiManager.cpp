#include "ImGuiManager.h"
#include <ImGuizmo.h>
#include "core/logging/Logger.h"


ImGuiManager::ImGuiManager()
    : initialized(false) {
}

ImGuiManager::~ImGuiManager() {
    Shutdown();
}

ImGuiManager &ImGuiManager::Instance() {
    static ImGuiManager instance;
    return instance;
}

bool ImGuiManager::Initialize(GLFWwindow *window) {
    if (initialized)
        return true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Multi-Viewport

    Theme::ApplyTheme(ThemeStyle::Enemymouse);

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    initialized = true;
    Logger::Log(LogLevel::INFO, "ImGui initialized");

    return true;
}

void ImGuiManager::Shutdown() {
    if (!initialized)
        return;

    Logger::Log(LogLevel::INFO, "ImGui shutdown started...");

    Logger::Log(LogLevel::INFO, "Shutting down viewports...");
    ImGui::DestroyPlatformWindows();

    Logger::Log(LogLevel::INFO, "Shutting down ImGui backends...");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    Logger::Log(LogLevel::INFO, "Destroying ImGui context...");
    ImGui::DestroyContext();

    initialized = false;

    Logger::Log(LogLevel::INFO, "ImGui shutdown completed");
}

void ImGuiManager::BeginFrame() {
    if (!initialized)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiManager::EndFrame() {
    if (!initialized)
        return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

bool ImGuiManager::IsInitialized() const {
    return initialized;
}

void ImGuiManager::SetTheme(ThemeStyle t) {
    theme = t;
}

ThemeStyle ImGuiManager::GetTheme() const {
    return theme;
}