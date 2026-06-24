#include "GLStateGuard.h"

GLStateGuard::GLStateGuard() {
    depthTest = glIsEnabled(GL_DEPTH_TEST);
    blend = glIsEnabled(GL_BLEND);
    cullFace = glIsEnabled(GL_CULL_FACE);
    scissorTest = glIsEnabled(GL_SCISSOR_TEST);

    glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
    glGetIntegerv(GL_VIEWPORT, viewport);
}

GLStateGuard::~GLStateGuard() {
    if (depthTest) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    if (blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);

    if (cullFace) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);

    if (scissorTest) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);

    glDepthFunc(depthFunc);
    glBlendFunc(blendSrc, blendDst);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}