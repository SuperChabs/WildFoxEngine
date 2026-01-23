export module WFE.Rendering.Commands.CommandBuffer;

import WFE.Rendering.Commands.RenderCommand;

import <vector>;
import <memory>;
import <algorithm>;

export class CommandBuffer
{
private:
    std::vector<std::unique_ptr<RenderCommand>> commands;
    
public:
    void Submit(std::unique_ptr<RenderCommand> cmd)
    {
        commands.push_back(std::move(cmd));
    }
    
    void Sort()
    {
        for (auto& cmd : commands)
            cmd->GenerateSortKey();
        
        std::sort(commands.begin(), commands.end(),
            [](const std::unique_ptr<RenderCommand>& a, 
               const std::unique_ptr<RenderCommand>& b) {
                return a->sortKey < b->sortKey;
            });
    }
    
    void Execute()
    {
        for (auto& cmd : commands)
            cmd->Execute();
    }
    
    void Clear()
    {
        commands.clear();
    }
    
    size_t GetCount() const { return commands.size(); }
};