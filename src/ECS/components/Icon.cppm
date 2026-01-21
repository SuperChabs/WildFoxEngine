module;

#include <string>

export module WFE.ECS.Components.Icon;

export struct IconComponent 
{
    std::string iconTexturePath;
    unsigned int textureID = 0;
    float scale = 0.5f;
    bool billboardMode = true; 
    
    IconComponent() = default;
    IconComponent(const std::string& path, float s = 0.5f) 
        : iconTexturePath(path), scale(s) {}
};