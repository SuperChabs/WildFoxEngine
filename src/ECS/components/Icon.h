#pragma once

#include <string>

struct c_Icon {
    std::string iconTexturePath;
    unsigned int textureID = 0;
    float scale = 0.5f;
    bool billboardMode = true;

    c_Icon() = default;

    c_Icon(const std::string &path, float s = 0.5f)
        : iconTexturePath(path), scale(s) {
    }
};