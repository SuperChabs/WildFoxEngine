#pragma once

#include <glad/glad.h>

class GLStateGuard {
private:
    bool depthTest;
    bool blend;
    bool cullFace;
    bool scissorTest;

    GLint depthFunc;
    GLint blendSrc;
    GLint blendDst;
    GLint viewport[4];

public:
    GLStateGuard();


    ~GLStateGuard();


    GLStateGuard(const GLStateGuard &) = delete;

    GLStateGuard &operator=(const GLStateGuard &) = delete;
};