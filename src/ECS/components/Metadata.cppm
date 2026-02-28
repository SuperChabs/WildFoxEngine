module;

#include <cstdint>
#include<string>

export module WFE.ECS.Components.Metadata;

export struct TagComponent 
{
    std::string name;
    
    TagComponent() = default;
    TagComponent(const std::string& n) : name(n) {}
};

export struct IDComponent 
{
    uint64_t id;
    
    IDComponent() = default;
    IDComponent(uint64_t i) : id(i) {}
};

export struct CameraTypeComponent
{
    enum class Type { EDITOR, GAME };
    Type type = Type::EDITOR;
    
    CameraTypeComponent() = default;
    CameraTypeComponent(Type t) : type(t) {}
};

export struct ModelComponent
{
    std::string filePath;

    ModelComponent() = default;
    ModelComponent(const std::string& path) : filePath(path) {}
};