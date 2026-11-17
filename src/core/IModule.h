#pragma once

class IModule {
protected:
    bool m_isInitialized = false;
    bool m_shouldUpdate = true;

    /// @brief Module-specific initialization, called by Initialize()
    /// @return true on success; the result becomes IsInitialized()
    virtual bool OnInitialize() = 0;

public:
    virtual ~IModule();

    bool Initialize() {
        m_isInitialized = OnInitialize();
        return m_isInitialized;
    }

    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;

    [[nodiscard]] virtual const char *GetName() const = 0;
    [[nodiscard]] virtual bool IsRequired() const = 0;
    [[nodiscard]] bool IsInitialized() const { return m_isInitialized; };
    [[nodiscard]] bool IsShouldUpdate() const { return m_shouldUpdate; };

    void SetShouldUpdate(const bool shouldUpdate) { m_shouldUpdate = shouldUpdate; }
};
