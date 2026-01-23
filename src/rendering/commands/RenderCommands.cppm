module;

#include <glm/glm.hpp>
#include <memory>
#include <functional>

export module WFE.Rendering.Commands.RenderCommand;

export enum class RenderCommandType
{
    DRAW_MESH,
    DRAW_SKYBOX,
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
    
    virtual void GenerateSortKey() { sortKey = 0; }
};