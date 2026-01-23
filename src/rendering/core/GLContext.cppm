module;

#include <glad/glad.h>
#include <string>

export module WFE.Rendering.Core.GLContext;

import WFE.Core.Logger;

export class GLContext
{
private:
    struct State
    {
        GLuint boundVAO = 0;
        GLuint boundVBO = 0;
        GLuint boundEBO = 0;
        GLuint boundFBO = 0;
        GLuint boundTexture2D = 0;
        GLuint boundShader = 0;
        
        bool depthTest = true;
        bool blend = false;
        bool cullFace = true;
        
        GLenum depthFunc = GL_LESS;
        GLenum cullMode = GL_BACK;
        
        int viewport[4] = {0, 0, 0, 0};
    } state;
    
    struct Stats
    {
        int drawCalls = 0;
        int stateChanges = 0;
        int vertexCount = 0;
        int triangleCount = 0;
        
        void Reset()
        {
            drawCalls = 0;
            stateChanges = 0;
            vertexCount = 0;
            triangleCount = 0;
        }
    } stats;

public:
    GLContext() = default;
    
    void BindVAO(GLuint vao)
    {
        if (state.boundVAO != vao)
        {
            glBindVertexArray(vao);
            state.boundVAO = vao;
            stats.stateChanges++;
        }
    }
    
    void UnbindVAO()
    {
        BindVAO(0);
    }
    
    void BindVBO(GLuint vbo)
    {
        if (state.boundVBO != vbo)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            state.boundVBO = vbo;
            stats.stateChanges++;
        }
    }
    
    void BindEBO(GLuint ebo)
    {
        if (state.boundEBO != ebo)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            state.boundEBO = ebo;
            stats.stateChanges++;
        }
    }
    
    void BindFBO(GLuint fbo)
    {
        if (state.boundFBO != fbo)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            state.boundFBO = fbo;
            stats.stateChanges++;
        }
    }
    
    void BindTexture2D(GLuint texture, GLuint slot = 0)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        if (state.boundTexture2D != texture)
        {
            glBindTexture(GL_TEXTURE_2D, texture);
            state.boundTexture2D = texture;
            stats.stateChanges++;
        }
    }
    
    void UseShader(GLuint shader)
    {
        if (state.boundShader != shader)
        {
            glUseProgram(shader);
            state.boundShader = shader;
            stats.stateChanges++;
        }
    }
    
    void SetDepthTest(bool enable)
    {
        if (state.depthTest != enable)
        {
            if (enable) glEnable(GL_DEPTH_TEST);
            else glDisable(GL_DEPTH_TEST);
            state.depthTest = enable;
            stats.stateChanges++;
        }
    }
    
    void SetDepthFunc(GLenum func)
    {
        if (state.depthFunc != func)
        {
            glDepthFunc(func);
            state.depthFunc = func;
            stats.stateChanges++;
        }
    }
    
    void SetBlend(bool enable)
    {
        if (state.blend != enable)
        {
            if (enable)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            else
            {
                glDisable(GL_BLEND);
            }
            state.blend = enable;
            stats.stateChanges++;
        }
    }
    
    void SetCullFace(bool enable)
    {
        if (state.cullFace != enable)
        {
            if (enable) glEnable(GL_CULL_FACE);
            else glDisable(GL_CULL_FACE);
            state.cullFace = enable;
            stats.stateChanges++;
        }
    }
    
    void SetCullMode(GLenum mode)
    {
        if (state.cullMode != mode)
        {
            glCullFace(mode);
            state.cullMode = mode;
            stats.stateChanges++;
        }
    }
    
    void SetViewport(int x, int y, int width, int height)
    {
        if (state.viewport[0] != x || state.viewport[1] != y ||
            state.viewport[2] != width || state.viewport[3] != height)
        {
            glViewport(x, y, width, height);
            state.viewport[0] = x;
            state.viewport[1] = y;
            state.viewport[2] = width;
            state.viewport[3] = height;
            stats.stateChanges++;
        }
    }
    
    void DrawArrays(GLenum mode, GLint first, GLsizei count)
    {
        glDrawArrays(mode, first, count);
        stats.drawCalls++;
        stats.vertexCount += count;
        if (mode == GL_TRIANGLES)
            stats.triangleCount += count / 3;
    }
    
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
    {
        glDrawElements(mode, count, type, indices);
        stats.drawCalls++;
        stats.vertexCount += count;
        if (mode == GL_TRIANGLES)
            stats.triangleCount += count / 3;
    }
    
    void Clear(GLbitfield mask)
    {
        glClear(mask);
    }
    
    void ClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }
    
    // Stats
    const Stats& GetStats() const { return stats; }
    void ResetStats() { stats.Reset(); }
    
    // State info
    const State& GetState() const { return state; }
    
    void LogStats() const
    {
        Logger::Log(LogLevel::DEBUG, 
            "Frame Stats - Draws: " + std::to_string(stats.drawCalls) +
            ", State changes: " + std::to_string(stats.stateChanges) +
            ", Vertices: " + std::to_string(stats.vertexCount) +
            ", Triangles: " + std::to_string(stats.triangleCount));
    }
};