module;

#include <vector>
#include <memory>
#include <algorithm>

export module WFE.Rendering.RenderQueue;

import WFE.Rendering.Commands.RenderCommand;
import WFE.Core.Logger;

export enum class RenderQueueType
{
    BACKGROUND = 0,   // Skybox, etc (1000)
    OPAQUE = 1000,    // Solid objects (2000-2999)
    TRANSPARENT = 3000, // Transparent objects (3000-3999)
    OVERLAY = 4000    // UI, Icons (4000+)
};

export class RenderQueue
{
private:
    std::vector<std::unique_ptr<RenderCommand>> commands;
    bool sorted = false;

public:
    void Submit(std::unique_ptr<RenderCommand> cmd)
    {
        if (!cmd)
        {
            Logger::Log(LogLevel::WARNING, "Attempting to submit null command to RenderQueue");
            return;
        }
        
        commands.push_back(std::move(cmd));
        sorted = false;
    }
    
    void Sort()
    {
        if (sorted) return;
        
        // Generate sort keys for all commands
        for (auto& cmd : commands)
        {
            cmd->GenerateSortKey();
        }
        
        // Sort by sort key (opaque front-to-back, transparent back-to-front)
        std::sort(commands.begin(), commands.end(),
            [](const std::unique_ptr<RenderCommand>& a, 
               const std::unique_ptr<RenderCommand>& b) 
            {
                return a->sortKey < b->sortKey;
            });
        
        sorted = true;
        
        Logger::Log(LogLevel::DEBUG, 
            "RenderQueue sorted " + std::to_string(commands.size()) + " commands");
    }
    
    void Execute()
    {
        if (!sorted)
        {
            Logger::Log(LogLevel::WARNING, "Executing unsorted RenderQueue");
            Sort();
        }
        
        for (auto& cmd : commands)
        {
            if (cmd)
            {
                cmd->Execute();
            }
        }
    }
    
    void Clear()
    {
        commands.clear();
        sorted = false;
    }
    
    size_t GetCommandCount() const { return commands.size(); }
    bool IsSorted() const { return sorted; }
};