#pragma once

#include <GLFW/glfw3.h>

/// @file Time.cppm
/// @brief Time manager
/// @author SuperChabs
/// @date 2026-01-28

class Time {
private:
    float currentFrame;
    float lastFrame;
    float deltaTime;
    float timeScale;

public:
    Time();


    void Update();


    float GetDeltaTime() const;

    float GetTime() const;

    float GetTimeScale() const;

    void SetTimeScale(float scale);


    float GetFPS() const;
};