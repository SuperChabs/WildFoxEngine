#pragma once

#include <memory>

#include "IModule.h"
#include "Input.h"
#include "Time.h"
#include "CommandManager.h"
#include "Window.h"
#include "EventBus.h"

class CoreModule : public IModule {
    std::unique_ptr<Input>          m_input;
    std::unique_ptr<Time>           m_time;
    std::unique_ptr<CommandManager> m_commandManager;
    std::unique_ptr<Window>         m_window;
    std::unique_ptr<EventBus>       m_eventBus;

public:
    CoreModule(int width, int height, const std::string &title);

    bool Initialize() override;

    void Update(float deltaTime) override;
    void Shutdown() override;

    [[nodiscard]]
    const char *GetName() const override { return "Core"; };
    [[nodiscard]]
    int GetPriority() const override { return 1; };
    [[nodiscard]]
    bool IsRequired() const override { return true; };

    [[nodiscard]]
    Input* GetInput() const { return m_input.get(); }
    [[nodiscard]]
    Time* GetTime() const { return m_time.get(); }
    [[nodiscard]]
    CommandManager* GetCommandManager() const { return m_commandManager.get(); }
    [[nodiscard]]
    Window* GetWindow() const { return m_window.get(); }
    [[nodiscard]]
    EventBus* GetEventBus() const { return m_eventBus.get(); }
};
