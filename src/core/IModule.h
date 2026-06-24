#pragma once

class IModule {
protected:
    bool isInitialized = false;

public:
    virtual ~IModule();

    virtual bool Initialize() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Shutdown() = 0;

    virtual const char *GetName() const = 0;

    virtual int GetPriority() const = 0;

    virtual bool IsRequired() const = 0;

    bool IsInitialized() const { return isInitialized; };
};
