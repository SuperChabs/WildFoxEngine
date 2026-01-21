module;

#include <imgui.h>

export module WFE.UI.Theme;

import WFE.Core.Logger;

export enum class ThemeStyle
{
    Dark,
    Light,
    Custom,
    RedAccent,
    Enemymouse,
    AdobeSpectrum,
    LedSynthmaster,
    DougbinksLight,
    DougbinksDark,
    UnrealEngine,
    Unity,
    VisualStudio
};

export class Theme
{
public:
    static void ApplyTheme(ThemeStyle style)
    {
        switch (style)
        {
            case ThemeStyle::Dark:
                ApplyDarkTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Dark");
                break;
            case ThemeStyle::Light:
                ApplyLightTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Light");
                break;
            case ThemeStyle::Custom:
                ApplyCustomTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Custom");
                break;
            case ThemeStyle::RedAccent:
                ApplyRedAccentTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Red Accent");
                break;
            case ThemeStyle::Enemymouse:
                ApplyEnemymouseTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Enemymouse (Cyan)");
                break;
            case ThemeStyle::AdobeSpectrum:
                ApplyAdobeSpectrumTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Adobe Spectrum");
                break;
            case ThemeStyle::LedSynthmaster:
                ApplyLedSynthmasterTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: LED Synthmaster");
                break;
            case ThemeStyle::DougbinksLight:
                ApplyDougbinksLightTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Dougbinks Light");
                break;
            case ThemeStyle::DougbinksDark:
                ApplyDougbinksDarkTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Dougbinks Dark");
                break;
            case ThemeStyle::UnrealEngine:
                ApplyUnrealEngineTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Unreal Engine");
                break;
            case ThemeStyle::Unity:
                ApplyUnityTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Unity");
                break;
            case ThemeStyle::VisualStudio:
                ApplyVisualStudioTheme();
                Logger::Log(LogLevel::INFO, LogCategory::UI, "Theme changed to: Visual Studio");
                break;
        }
    }

private:
    // ═══════════════════════════════════════════════════════
    // HELPER: Convert U32 color to ImVec4 (for Adobe Spectrum)
    // ═══════════════════════════════════════════════════════
    static ImVec4 ColorConvertU32ToFloat4(unsigned int color)
    {
        float r = ((color >> 0) & 0xFF) / 255.0f;
        float g = ((color >> 8) & 0xFF) / 255.0f;
        float b = ((color >> 16) & 0xFF) / 255.0f;
        float a = ((color >> 24) & 0xFF) / 255.0f;
        return ImVec4(r, g, b, a);
    }

    // Adobe Spectrum Color Palette
    struct Spectrum
    {
        static constexpr unsigned int GRAY50  = 0xFFFFFFFF;
        static constexpr unsigned int GRAY75  = 0xFFFAFAFA;
        static constexpr unsigned int GRAY100 = 0xFFF5F5F5;
        static constexpr unsigned int GRAY200 = 0xFFEAEAEA;
        static constexpr unsigned int GRAY300 = 0xFFE1E1E1;
        static constexpr unsigned int GRAY400 = 0xFFCACACA;
        static constexpr unsigned int GRAY500 = 0xFFB3B3B3;
        static constexpr unsigned int GRAY600 = 0xFF8E8E8E;
        static constexpr unsigned int GRAY700 = 0xFF6E6E6E;
        static constexpr unsigned int GRAY800 = 0xFF4B4B4B;
        static constexpr unsigned int GRAY900 = 0xFF2C2C2C;
        static constexpr unsigned int BLUE400 = 0xFF378EF0;
        static constexpr unsigned int BLUE500 = 0xFF2680EB;
        static constexpr unsigned int BLUE600 = 0xFF1473E6;
        
        struct Static
        {
            static constexpr unsigned int NONE = 0x00000000;
        };
    };

    // ═══════════════════════════════════════════════════════
    // DARK THEME (Default ImGui Dark)
    // ═══════════════════════════════════════════════════════
    static void ApplyDarkTheme()
    {
        ImGui::StyleColorsDark();
    }

    // ═══════════════════════════════════════════════════════
    // LIGHT THEME (Default ImGui Light)
    // ═══════════════════════════════════════════════════════
    static void ApplyLightTheme()
    {
        ImGui::StyleColorsLight();
    }

    // ═══════════════════════════════════════════════════════
    // CUSTOM THEME (Ваша власна тема)
    // ═══════════════════════════════════════════════════════
    static void ApplyCustomTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Основні кольори
        colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        colors[ImGuiCol_Header]                 = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        // Стилі
        style.WindowPadding     = ImVec2(8.00f, 8.00f);
        style.FramePadding      = ImVec2(5.00f, 2.00f);
        style.CellPadding       = ImVec2(6.00f, 6.00f);
        style.ItemSpacing       = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing  = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
        style.IndentSpacing     = 25;
        style.ScrollbarSize     = 15;
        style.GrabMinSize       = 10;
        style.WindowBorderSize  = 1;
        style.ChildBorderSize   = 1;
        style.PopupBorderSize   = 1;
        style.FrameBorderSize   = 1;
        style.TabBorderSize     = 1;
        style.WindowRounding    = 7;
        style.ChildRounding     = 4;
        style.FrameRounding     = 3;
        style.PopupRounding     = 4;
        style.ScrollbarRounding = 9;
        style.GrabRounding      = 3;
        style.LogSliderDeadzone = 4;
        style.TabRounding       = 4;
    }

    // ═══════════════════════════════════════════════════════
    // RED ACCENT DARK THEME
    // ═══════════════════════════════════════════════════════
    static void ApplyRedAccentTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Стилі
        style.WindowMinSize        = ImVec2(160, 20);
        style.FramePadding         = ImVec2(4, 2);
        style.ItemSpacing          = ImVec2(6, 2);
        style.ItemInnerSpacing     = ImVec2(2, 4);
        style.Alpha                = 0.95f;
        style.WindowRounding       = 4.0f;
        style.FrameRounding        = 2.0f;
        style.IndentSpacing        = 6.0f;
        style.ColumnsMinSpacing    = 50.0f;
        style.GrabMinSize          = 14.0f;
        style.GrabRounding         = 16.0f;
        style.ScrollbarSize        = 12.0f;
        style.ScrollbarRounding    = 16.0f;

        // Кольори
        colors[ImGuiCol_Text]                   = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.20f, 0.22f, 0.27f, 0.90f);
        colors[ImGuiCol_Border]                 = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.92f, 0.18f, 0.29f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
    }

    // ═══════════════════════════════════════════════════════
    // ENEMYMOUSE THEME (Cyan/Teal Cyberpunk)
    // ═══════════════════════════════════════════════════════
    static void ApplyEnemymouseTheme() 
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        // Стилі (оновлено для сучасного ImGui)
        style.Alpha = 1.0f;
        style.ChildRounding = 3.0f;
        style.WindowRounding = 3.0f;
        style.GrabRounding = 1.0f;
        style.GrabMinSize = 20.0f;
        style.FrameRounding = 3.0f;

        // Кольори (оновлено назви для сучасного ImGui)
        colors[ImGuiCol_Text]                   = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.00f, 0.13f, 0.13f, 0.90f);
        colors[ImGuiCol_Border]                 = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
        colors[ImGuiCol_Button]                 = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
        colors[ImGuiCol_Header]                 = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
        colors[ImGuiCol_Separator]              = ImVec4(0.00f, 0.50f, 0.50f, 0.33f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.00f, 0.50f, 0.50f, 0.47f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.00f, 0.70f, 0.70f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.50f, 0.50f, 0.86f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.00f, 1.00f, 1.00f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.00f, 0.78f, 0.78f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.40f, 0.40f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.00f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.00f, 1.00f, 1.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
    }

    // ═══════════════════════════════════════════════════════
    // ADOBE SPECTRUM THEME (Light Professional)
    // ═══════════════════════════════════════════════════════
    static void ApplyAdobeSpectrumTheme() 
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        style.GrabRounding = 4.0f;
        style.FrameRounding = 2.0f;
        style.WindowRounding = 0.0f;

        colors[ImGuiCol_Text]                   = ColorConvertU32ToFloat4(Spectrum::GRAY800);
        colors[ImGuiCol_TextDisabled]           = ColorConvertU32ToFloat4(Spectrum::GRAY500);
        colors[ImGuiCol_WindowBg]               = ColorConvertU32ToFloat4(Spectrum::GRAY100);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ColorConvertU32ToFloat4(Spectrum::GRAY50);
        colors[ImGuiCol_Border]                 = ColorConvertU32ToFloat4(Spectrum::GRAY300);
        colors[ImGuiCol_BorderShadow]           = ColorConvertU32ToFloat4(Spectrum::Static::NONE);
        colors[ImGuiCol_FrameBg]                = ColorConvertU32ToFloat4(Spectrum::GRAY75);
        colors[ImGuiCol_FrameBgHovered]         = ColorConvertU32ToFloat4(Spectrum::GRAY50);
        colors[ImGuiCol_FrameBgActive]          = ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_TitleBg]                = ColorConvertU32ToFloat4(Spectrum::GRAY300);
        colors[ImGuiCol_TitleBgActive]          = ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_TitleBgCollapsed]       = ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_MenuBarBg]              = ColorConvertU32ToFloat4(Spectrum::GRAY100);
        colors[ImGuiCol_ScrollbarBg]            = ColorConvertU32ToFloat4(Spectrum::GRAY100);
        colors[ImGuiCol_ScrollbarGrab]          = ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ColorConvertU32ToFloat4(Spectrum::GRAY600);
        colors[ImGuiCol_ScrollbarGrabActive]    = ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_CheckMark]              = ColorConvertU32ToFloat4(Spectrum::BLUE500);
        colors[ImGuiCol_SliderGrab]             = ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_SliderGrabActive]       = ColorConvertU32ToFloat4(Spectrum::GRAY800);
        colors[ImGuiCol_Button]                 = ColorConvertU32ToFloat4(Spectrum::GRAY75);
        colors[ImGuiCol_ButtonHovered]          = ColorConvertU32ToFloat4(Spectrum::GRAY50);
        colors[ImGuiCol_ButtonActive]           = ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_Header]                 = ColorConvertU32ToFloat4(Spectrum::BLUE400);
        colors[ImGuiCol_HeaderHovered]          = ColorConvertU32ToFloat4(Spectrum::BLUE500);
        colors[ImGuiCol_HeaderActive]           = ColorConvertU32ToFloat4(Spectrum::BLUE600);
        colors[ImGuiCol_Separator]              = ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_SeparatorHovered]       = ColorConvertU32ToFloat4(Spectrum::GRAY600);
        colors[ImGuiCol_SeparatorActive]        = ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_ResizeGrip]             = ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_ResizeGripHovered]      = ColorConvertU32ToFloat4(Spectrum::GRAY600);
        colors[ImGuiCol_ResizeGripActive]       = ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_Tab]                    = ColorConvertU32ToFloat4(Spectrum::GRAY75);
        colors[ImGuiCol_TabHovered]             = ColorConvertU32ToFloat4(Spectrum::BLUE500);
        colors[ImGuiCol_TabActive]              = ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_TabUnfocused]           = ColorConvertU32ToFloat4(Spectrum::GRAY75);
        colors[ImGuiCol_TabUnfocusedActive]     = ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_DockingPreview]         = ColorConvertU32ToFloat4(Spectrum::BLUE400);
        colors[ImGuiCol_DockingEmptyBg]         = ColorConvertU32ToFloat4(Spectrum::GRAY100);
        colors[ImGuiCol_PlotLines]              = ColorConvertU32ToFloat4(Spectrum::BLUE400);
        colors[ImGuiCol_PlotLinesHovered]       = ColorConvertU32ToFloat4(Spectrum::BLUE600);
        colors[ImGuiCol_PlotHistogram]          = ColorConvertU32ToFloat4(Spectrum::BLUE400);
        colors[ImGuiCol_PlotHistogramHovered]   = ColorConvertU32ToFloat4(Spectrum::BLUE600);
        colors[ImGuiCol_TextSelectedBg]         = ColorConvertU32ToFloat4((Spectrum::BLUE400 & 0x00FFFFFF) | 0x33000000);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ColorConvertU32ToFloat4((Spectrum::GRAY900 & 0x00FFFFFF) | 0x0A000000);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    }

    // ═══════════════════════════════════════════════════════
    // LED SYNTHMASTER THEME (Light Green Accent)
    // ═══════════════════════════════════════════════════════
    static void ApplyLedSynthmasterTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        // Стилі
        style.WindowPadding     = ImVec2(15, 15);
        style.WindowRounding    = 5.0f;
        style.FramePadding      = ImVec2(5, 5);
        style.FrameRounding     = 4.0f;
        style.ItemSpacing       = ImVec2(12, 8);
        style.ItemInnerSpacing  = ImVec2(8, 6);
        style.IndentSpacing     = 25.0f;
        style.ScrollbarSize     = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize       = 5.0f;
        style.GrabRounding      = 3.0f;

        // Кольори (оновлено для сучасного ImGui)
        colors[ImGuiCol_Text]                   = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.40f, 0.39f, 0.38f, 0.77f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.92f, 0.91f, 0.88f, 0.70f);
        colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 0.98f, 0.95f, 0.58f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.92f, 0.91f, 0.88f, 0.92f);
        colors[ImGuiCol_Border]                 = ImVec4(0.84f, 0.83f, 0.80f, 0.65f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.99f, 1.00f, 0.40f, 0.78f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(1.00f, 0.98f, 0.95f, 0.47f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.00f, 0.00f, 0.00f, 0.21f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.90f, 0.91f, 0.00f, 0.78f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.25f, 1.00f, 0.00f, 0.80f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.99f, 1.00f, 0.22f, 0.86f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.25f, 1.00f, 0.00f, 0.76f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.25f, 1.00f, 0.00f, 0.86f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.00f, 0.00f, 0.00f, 0.32f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.88f, 0.88f, 0.88f, 0.86f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.25f, 1.00f, 0.00f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.92f, 0.91f, 0.88f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.25f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
    }

    // ═══════════════════════════════════════════════════════
    // DOUGBINKS LIGHT THEME
    // ═══════════════════════════════════════════════════════
    static void ApplyDougbinksLightTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        style.Alpha = 1.0f;
        style.FrameRounding = 3.0f;
        
        colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.76f, 0.80f, 0.84f, 0.93f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.60f, 0.73f, 0.88f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.92f, 0.93f, 0.94f, 0.98f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.26f, 0.59f, 0.98f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    }

    // ═══════════════════════════════════════════════════════
    // DOUGBINKS DARK THEME
    // ═══════════════════════════════════════════════════════
    static void ApplyDougbinksDarkTheme()
    {
        // Спочатку застосовуємо світлу тему
        ApplyDougbinksLightTheme();
        
        // Потім інвертуємо кольори для темної теми
        ImGuiStyle& style = ImGui::GetStyle();
        
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            ImVec4& col = style.Colors[i];
            float H, S, V;
            ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

            if (S < 0.1f) // Якщо насиченість низька (сірий колір)
            {
                V = 1.0f - V; // Інвертуємо яскравість
            }
            ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
        }
    }

    // ═══════════════════════════════════════════════════════
    // UNREAL ENGINE THEME
    // ═══════════════════════════════════════════════════════
    static void ApplyUnrealEngineTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.16f, 0.16f, 0.16f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);

        style.WindowRounding    = 0.0f;
        style.ChildRounding     = 0.0f;
        style.FrameRounding     = 0.0f;
        style.PopupRounding     = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabRounding      = 0.0f;
        style.TabRounding       = 0.0f;
    }

    // ═══════════════════════════════════════════════════════
    // UNITY THEME
    // ═══════════════════════════════════════════════════════
    static void ApplyUnityTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.50f, 0.80f, 1.00f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);

        style.WindowRounding    = 0.0f;
        style.ChildRounding     = 3.0f;
        style.FrameRounding     = 3.0f;
        style.GrabRounding      = 3.0f;
    }

    // ═══════════════════════════════════════════════════════
    // VISUAL STUDIO THEME
    // ═══════════════════════════════════════════════════════
    static void ApplyVisualStudioTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Text]                   = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.45f, 0.45f, 0.47f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.27f, 0.27f, 0.29f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.37f, 0.37f, 0.39f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.48f, 0.80f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.48f, 0.80f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);

        style.WindowRounding    = 0.0f;
        style.FrameRounding     = 2.0f;
    }
};