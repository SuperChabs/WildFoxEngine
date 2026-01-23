module;

#include <glad/glad.h>

export module WFE.Rendering.Core.GLStateGuard;

import WFE.Core.Logger;

export class GLStateGuard
{
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
    GLStateGuard()
    {
        depthTest = glIsEnabled(GL_DEPTH_TEST);
        blend = glIsEnabled(GL_BLEND);
        cullFace = glIsEnabled(GL_CULL_FACE);
        scissorTest = glIsEnabled(GL_SCISSOR_TEST);
        
        glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
        glGetIntegerv(GL_VIEWPORT, viewport);
    }
    
    ~GLStateGuard()
    {
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
    
    GLStateGuard(const GLStateGuard&) = delete;
    GLStateGuard& operator=(const GLStateGuard&) = delete;
};

export class DepthTestGuard
{
    bool wasEnabled;
public:
    DepthTestGuard(bool enable)
    {
        wasEnabled = glIsEnabled(GL_DEPTH_TEST);
        if (enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }
    
    ~DepthTestGuard()
    {
        if (wasEnabled) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }
};

export class BlendGuard
{
    bool wasEnabled;
public:
    BlendGuard(bool enable)
    {
        wasEnabled = glIsEnabled(GL_BLEND);
        if (enable)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
    
    ~BlendGuard()
    {
        if (wasEnabled) glEnable(GL_BLEND);
        else glDisable(GL_BLEND);
    }
};