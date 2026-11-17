#pragma once

/// @file Events.h
/// @brief Names of all EventBus events, so a typo becomes a compile error

namespace Events {
    // notifications
    inline constexpr const char *PlayModeStarted = "play_mode_started"; ///< payload: std::string scene path
    inline constexpr const char *PlayModeStopped = "play_mode_stopped";
    inline constexpr const char *DebugPaused = "debug_paused";
    inline constexpr const char *DebugResumed = "debug_resumed";
    inline constexpr const char *TriggerEnter = "trigger_enter"; ///< payload: std::pair<entt::entity, entt::entity>

    // requests
    inline constexpr const char *DebugPause = "debug_pause";
    inline constexpr const char *DebugResume = "debug_resume";
}
