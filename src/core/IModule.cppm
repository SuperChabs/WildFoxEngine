export module WFE.Core.IModule;

export class IModule
{
public:
    virtual ~IModule() = default;
    virtual bool Initialize() = 0;
    virtual void Update(float deltaTime) {}
    virtual void Shutdown() = 0;
    virtual const char* GetName() const = 0;
    virtual int GetPriority() const { return 100; }
    virtual bool IsRequired() const { return true; }
};