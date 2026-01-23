module;

#include <chrono>

export module WFE.Rendering.RenderStats;

import WFE.Core.Logger;

export class RenderStats
{
public:
    int drawCalls = 0;
    int stateChanges = 0;
    int vertexCount = 0;
    int triangleCount = 0;
    
    float frameTime = 0.0f;
    float fps = 0.0f;
    
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    
    TimePoint frameStart;
    TimePoint lastFPSUpdate;
    int frameCount = 0;
    int fpsFrameCount = 0;

public:
    void BeginFrame()
    {
        frameStart = Clock::now();
    }
    
    void EndFrame()
    {
        auto now = Clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            now - frameStart
        );
        frameTime = duration.count() / 1000.0f; // ms
        
        frameCount++;
        fpsFrameCount++;
        
        // Update FPS every second
        auto fpsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastFPSUpdate
        );
        
        if (fpsDuration.count() >= 1000)
        {
            fps = fpsFrameCount / (fpsDuration.count() / 1000.0f);
            fpsFrameCount = 0;
            lastFPSUpdate = now;
        }
    }
    
    void Reset()
    {
        drawCalls = 0;
        stateChanges = 0;
        vertexCount = 0;
        triangleCount = 0;
        frameTime = 0.0f;
    }
    
    int GetFrameCount() const { return frameCount; }
    
    void LogFPS() const
    {
        Logger::Log(LogLevel::DEBUG, 
            "FPS: " + std::to_string(static_cast<int>(fps)) + 
            " (" + std::to_string(frameTime) + "ms)");
    }
};