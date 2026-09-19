#pragma once

class IModule {
protected:
    bool m_isInitialized = false;
    bool m_shouldUpdate = true;

public:
    virtual ~IModule();

    virtual bool Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;

    [[nodiscard]] virtual const char *GetName() const = 0;
    [[nodiscard]] virtual int GetPriority() const = 0;
    [[nodiscard]] virtual bool IsRequired() const = 0;
    [[nodiscard]] bool IsInitialized() const { return m_isInitialized; };
    [[nodiscard]] bool IsShouldUpdate() const { return m_shouldUpdate; };

    void SetShouldUpdate(const bool shouldUpdate) { m_shouldUpdate = shouldUpdate; }
};