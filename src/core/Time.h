#pragma once

/// @file Time.h
/// @brief Time manager
/// @author SuperChabs
/// @date 2026-01-28

class Time {
    float currentFrame;
    float lastFrame;
    float deltaTime;
    float timeScale;

public:
    Time();

    void Update();

    [[nodiscard]] float GetDeltaTime() const;
    [[nodiscard]] float GetTime() const;
    [[nodiscard]] float GetTimeScale() const;
    [[nodiscard]] float GetFPS() const;

    void SetTimeScale(float scale);
};