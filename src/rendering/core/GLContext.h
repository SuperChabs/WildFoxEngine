#pragma once

#include <glad/glad.h>

class GLContext {
private:
    struct State {
        GLuint boundVAO = 0;
        GLuint boundVBO = 0;
        GLuint boundEBO = 0;
        GLuint boundFBO = 0;
        GLuint boundTexture2D = 0;
        GLuint boundShader = 0;

        bool depthTest = true;
        bool blend = false;
        bool cullFace = false;

        GLenum depthFunc = GL_LESS;
        GLenum cullMode = GL_BACK;

        int viewport[4] = {0, 0, 0, 0};
    } state;

    struct Stats {
        int drawCalls = 0;
        int stateChanges = 0;
        int vertexCount = 0;
        int triangleCount = 0;

        void Reset() {
            drawCalls = 0;
            stateChanges = 0;
            vertexCount = 0;
            triangleCount = 0;
        }
    } stats;

public:
    GLContext() = default;

    void BindVAO(GLuint vao);

    void UnbindVAO();

    void BindVBO(GLuint vbo);

    void BindEBO(GLuint ebo);

    void BindFBO(GLuint fbo);

    void BindTexture2D(GLuint texture, GLuint slot = 0);

    void UseShader(GLuint shader);

    void SetDepthTest(bool enable);

    void SetDepthFunc(GLenum func);

    void SetBlend(bool enable);

    void SetCullFace(bool enable);

    void SetCullMode(GLenum mode);

    void SetViewport(int x, int y, int width, int height);

    void DrawArrays(GLenum mode, GLint first, GLsizei count);

    void DrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);

    void Clear(GLbitfield mask);

    void ClearColor(float r, float g, float b, float a);

    const Stats &GetStats() const;

    void ResetStats();

    const State &GetState() const;
};