#pragma once

class IModule {
protected:
    bool isInitialized = false;

public:
    virtual ~IModule();

    virtual bool Initialize() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Shutdown() = 0;

    [[nodiscard]] virtual const char *GetName() const = 0;

    [[nodiscard]] virtual int GetPriority() const = 0;

    [[nodiscard]] virtual bool IsRequired() const = 0;

    [[nodiscard]] bool IsInitialized() const { return isInitialized; };
};