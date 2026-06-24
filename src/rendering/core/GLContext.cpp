#include "GLContext.h"

void GLContext::BindVAO(GLuint vao) {
    if (state.boundVAO != vao) {
        glBindVertexArray(vao);
        state.boundVAO = vao;
        stats.stateChanges++;
    }
}

void GLContext::UnbindVAO() {
    BindVAO(0);
}

void GLContext::BindVBO(GLuint vbo) {
    if (state.boundVBO != vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        state.boundVBO = vbo;
        stats.stateChanges++;
    }
}

void GLContext::BindEBO(GLuint ebo) {
    if (state.boundEBO != ebo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        state.boundEBO = ebo;
        stats.stateChanges++;
    }
}

void GLContext::BindFBO(GLuint fbo) {
    if (state.boundFBO != fbo) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        state.boundFBO = fbo;
        stats.stateChanges++;
    }
}

void GLContext::BindTexture2D(GLuint texture, GLuint slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    if (state.boundTexture2D != texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        state.boundTexture2D = texture;
        stats.stateChanges++;
    }
}

void GLContext::UseShader(GLuint shader) {
    if (state.boundShader != shader) {
        glUseProgram(shader);
        state.boundShader = shader;
        stats.stateChanges++;
    }
}

void GLContext::SetDepthTest(bool enable) {
    if (state.depthTest != enable) {
        if (enable) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
        state.depthTest = enable;
        stats.stateChanges++;
    }
}

void GLContext::SetDepthFunc(GLenum func) {
    if (state.depthFunc != func) {
        glDepthFunc(func);
        state.depthFunc = func;
        stats.stateChanges++;
    }
}

void GLContext::SetBlend(bool enable) {
    if (state.blend != enable) {
        if (enable) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }
        state.blend = enable;
        stats.stateChanges++;
    }
}

void GLContext::SetCullFace(bool enable) {
    if (state.cullFace != enable) {
        if (enable) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
        state.cullFace = enable;
        stats.stateChanges++;
    }
}

void GLContext::SetCullMode(GLenum mode) {
    if (state.cullMode != mode) {
        glCullFace(mode);
        state.cullMode = mode;
        stats.stateChanges++;
    }
}

void GLContext::SetViewport(int x, int y, int width, int height) {
    if (state.viewport[0] != x || state.viewport[1] != y ||
        state.viewport[2] != width || state.viewport[3] != height) {
        glViewport(x, y, width, height);
        state.viewport[0] = x;
        state.viewport[1] = y;
        state.viewport[2] = width;
        state.viewport[3] = height;
        stats.stateChanges++;
    }
}

void GLContext::DrawArrays(GLenum mode, GLint first, GLsizei count) {
    glDrawArrays(mode, first, count);
    stats.drawCalls++;
    stats.vertexCount += count;
    if (mode == GL_TRIANGLES)
        stats.triangleCount += count / 3;
}

void GLContext::DrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
    glDrawElements(mode, count, type, indices);
    stats.drawCalls++;
    stats.vertexCount += count;
    if (mode == GL_TRIANGLES)
        stats.triangleCount += count / 3;
}

void GLContext::Clear(GLbitfield mask) {
    glClear(mask);
}

void GLContext::ClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

const GLContext::Stats &GLContext::GetStats() const {
    return stats;
}

void GLContext::ResetStats() {
    stats.Reset();
}

const GLContext::State &GLContext::GetState() const {
    return state;
}