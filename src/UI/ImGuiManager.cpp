#include "ImGuiManager.h"

#include "core/logging/Logger.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ImGuizmo.h"

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

    RedFoxTheme();

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

void ImGuiManager::BeginFrame(bool cameraControl) {
    if (!initialized)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    if (cameraControl) {
        ImGui::GetIO().MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
        ImGui::GetIO().WantCaptureMouse = false;
        ImGui::GetIO().WantCaptureKeyboard = false;
    }
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

void ImGuiManager::RedFoxTheme() {
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *c = style.Colors;

    const ImVec4 bg0 = ImVec4(0.09f, 0.06f, 0.05f, 1.00f);
    const ImVec4 bg1 = ImVec4(0.13f, 0.09f, 0.07f, 1.00f);
    const ImVec4 bg2 = ImVec4(0.18f, 0.12f, 0.09f, 1.00f);
    const ImVec4 bg3 = ImVec4(0.24f, 0.16f, 0.12f, 1.00f);

    const ImVec4 cream = ImVec4(0.98f, 0.92f, 0.82f, 1.00f);
    const ImVec4 creamDim = ImVec4(0.72f, 0.66f, 0.58f, 1.00f);

    const ImVec4 fox = ImVec4(0.86f, 0.38f, 0.12f, 1.00f);
    const ImVec4 foxHi = ImVec4(0.98f, 0.52f, 0.20f, 1.00f);
    const ImVec4 foxLo = ImVec4(0.62f, 0.26f, 0.08f, 1.00f);

    const ImVec4 border = ImVec4(0.32f, 0.18f, 0.11f, 0.85f);
    const ImVec4 borderHi = ImVec4(0.50f, 0.28f, 0.18f, 0.95f);

    c[ImGuiCol_Text] = cream;
    c[ImGuiCol_TextDisabled] = creamDim;

    c[ImGuiCol_WindowBg] = bg0;
    c[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_PopupBg] = bg1;

    c[ImGuiCol_Border] = border;
    c[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    c[ImGuiCol_FrameBg] = bg2;
    c[ImGuiCol_FrameBgHovered] = bg3;
    c[ImGuiCol_FrameBgActive] = ImVec4(bg3.x + 0.03f, bg3.y + 0.02f, bg3.z + 0.02f, 1.0f);

    c[ImGuiCol_TitleBg] = bg0;
    c[ImGuiCol_TitleBgActive] = foxLo;
    c[ImGuiCol_TitleBgCollapsed] = bg0;

    c[ImGuiCol_MenuBarBg] = bg1;

    c[ImGuiCol_ScrollbarBg] = bg0;
    c[ImGuiCol_ScrollbarGrab] = foxLo;
    c[ImGuiCol_ScrollbarGrabHovered] = fox;
    c[ImGuiCol_ScrollbarGrabActive] = foxHi;

    c[ImGuiCol_CheckMark] = foxHi;
    c[ImGuiCol_SliderGrab] = fox;
    c[ImGuiCol_SliderGrabActive] = foxHi;

    c[ImGuiCol_Button] = ImVec4(foxLo.x, foxLo.y, foxLo.z, 0.85f);
    c[ImGuiCol_ButtonHovered] = ImVec4(fox.x, fox.y, fox.z, 0.95f);
    c[ImGuiCol_ButtonActive] = foxHi;

    c[ImGuiCol_Header] = ImVec4(foxLo.x, foxLo.y, foxLo.z, 0.75f);
    c[ImGuiCol_HeaderHovered] = ImVec4(fox.x, fox.y, fox.z, 0.90f);
    c[ImGuiCol_HeaderActive] = foxHi;

    c[ImGuiCol_Separator] = border;
    c[ImGuiCol_SeparatorHovered] = borderHi;
    c[ImGuiCol_SeparatorActive] = fox;

    c[ImGuiCol_ResizeGrip] = foxLo;
    c[ImGuiCol_ResizeGripHovered] = fox;
    c[ImGuiCol_ResizeGripActive] = foxHi;

    c[ImGuiCol_Tab] = bg1;
    c[ImGuiCol_TabHovered] = fox;
    c[ImGuiCol_TabActive] = foxLo;
    c[ImGuiCol_TabUnfocused] = bg1;
    c[ImGuiCol_TabUnfocusedActive] = bg2;

    c[ImGuiCol_PlotLines] = cream;
    c[ImGuiCol_PlotLinesHovered] = foxHi;
    c[ImGuiCol_PlotHistogram] = fox;
    c[ImGuiCol_PlotHistogramHovered] = foxHi;

    c[ImGuiCol_TextSelectedBg] = ImVec4(fox.x, fox.y, fox.z, 0.35f);
    c[ImGuiCol_DragDropTarget] = ImVec4(foxHi.x, foxHi.y, foxHi.z, 0.9f);
    c[ImGuiCol_NavHighlight] = foxHi;
    c[ImGuiCol_NavWindowingHighlight] = ImVec4(cream.x, cream.y, cream.z, 0.7f);
    c[ImGuiCol_NavWindowingDimBg] = ImVec4(0, 0, 0, 0.5f);
    c[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.6f);

    style.WindowRounding = 8.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;

    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(10, 5);
    style.ItemSpacing = ImVec2(10, 7);
    style.ItemInnerSpacing = ImVec2(6, 5);
    style.IndentSpacing = 22.0f;

    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
}