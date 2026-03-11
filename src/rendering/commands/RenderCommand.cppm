module;

#include <glm/glm.hpp>
#include <memory>
#include <functional>

export module WFE.Rendering.Commands.RenderCommand;

export enum class RenderCommandType
{
    DRAW_MESH,
    DRAW_SKYBOX,
    DRAW_ICON,
    CLEAR,
    SET_VIEWPORT,
    SET_SHADER,
    SET_FRAMEBUFFER,
    CUSTOM
};

export class RenderCommand
{
public:
    RenderCommandType type;
    uint64_t sortKey = 0;
    
    RenderCommand(RenderCommandType t) : type(t) {}
    virtual ~RenderCommand() = default;
    
    virtual void Execute() = 0;
    
    virtual void GenerateSortKey()
    {
        sortKey = 0;
    }
    
    static uint64_t BuildSortKey(uint16_t queue, uint32_t material, uint32_t depth)
    {
        return (static_cast<uint64_t>(queue) << 48) |
               (static_cast<uint64_t>(material & 0xFFFFFF) << 24) |
               static_cast<uint64_t>(depth & 0xFFFFFF);
    }
};