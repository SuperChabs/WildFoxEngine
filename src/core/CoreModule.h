#pragma once

#include <memory>
#include <string>

#include "IModule.h"
#include "Input.h"
#include "Time.h"
#include "Window.h"

class CoreModule : public IModule {
    std::unique_ptr<Input> m_input;
    std::unique_ptr<Time> m_time;
    std::unique_ptr<Window> m_window;

    int m_width;
    int m_height;
    const char* m_title;

public:
    CoreModule(int width, int height, const char* title);

    bool OnInitialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    [[nodiscard]]
    const char *GetName() const override { return "Core"; };

    [[nodiscard]]
    bool IsRequired() const override { return true; };

    [[nodiscard]]
    Input *GetInput() const { return m_input.get(); }

    [[nodiscard]]
    Time *GetTime() const { return m_time.get(); }

    [[nodiscard]]
    Window *GetWindow() const { return m_window.get(); }
};
