/**
 * @file PostPass.cpp
 * @brief
 *
 * @author SuperChabs
 * @date 26.11.26
 */

#include "PostPass.h"

#include <algorithm>
#include <string>

static constexpr int SCENE_COLOR_SLOT = 0;

static glm::vec3 Hex(const unsigned int rgb) {
    return glm::vec3((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF) / 255.0f;
}

PostPass::PostPass(ShaderManager *sm) : RenderPass("PostPass", sm), m_settings(MakePreset(DitherStyle::Retro16)) {
    glGenVertexArrays(1, &m_emptyVAO);
}

PostPass::~PostPass() {
    if (m_emptyVAO != 0)
        glDeleteVertexArrays(1, &m_emptyVAO);
}

void PostPass::Setup() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
}

void PostPass::Execute(FrameContext &ctx) {
    if (!m_enabled || ctx.sceneColor == 0) return;
    Setup();

    BindTarget(ctx.targetFBO, ctx.width, ctx.height);

    const std::string shaderName = "post_dither";
    const auto &s = m_settings;

    m_shaderManager->Bind(shaderName);
    m_shaderManager->SetInt(shaderName, "sceneColor", SCENE_COLOR_SLOT);
    m_shaderManager->SetVec2(shaderName, "screenSize", glm::vec2(ctx.width, ctx.height));

    m_shaderManager->SetFloat(shaderName, "virtualHeight", s.virtualHeight);
    m_shaderManager->SetInt(shaderName, "mode", static_cast<int>(s.mode));
    m_shaderManager->SetFloat(shaderName, "colorLevels", static_cast<float>((1 << std::clamp(s.colorBits, 1, 8)) - 1));
    m_shaderManager->SetFloat(shaderName, "ditherSpread", s.ditherSpread);

    const int paletteSize = std::min(static_cast<int>(s.palette.size()), MAX_PALETTE);
    m_shaderManager->SetInt(shaderName, "paletteSize", paletteSize);
    for (int i = 0; i < paletteSize; i++)
        m_shaderManager->SetVec3(shaderName, "palette[" + std::to_string(i) + "]", s.palette[i]);

    m_shaderManager->SetFloat(shaderName, "exposure", s.exposure);
    m_shaderManager->SetFloat(shaderName, "contrast", s.contrast);
    m_shaderManager->SetFloat(shaderName, "gamma", s.gamma);
    m_shaderManager->SetFloat(shaderName, "vignette", s.vignette);

    m_shaderManager->SetFloat(shaderName, "outlineThreshold", s.outlineThreshold);
    m_shaderManager->SetVec3(shaderName, "outlineColor", s.outlineColor);

    m_shaderManager->SetFloat(shaderName, "pixelGrid", s.pixelGrid);

    glActiveTexture(GL_TEXTURE0 + SCENE_COLOR_SLOT);
    glBindTexture(GL_TEXTURE_2D, ctx.sceneColor);

    glBindVertexArray(m_emptyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    m_shaderManager->Unbind();

    Cleanup();
}

void PostPass::Cleanup() {
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

DitherSettings PostPass::MakePreset(const DitherStyle style) {
    DitherSettings s;

    switch (style) {
        case DitherStyle::PS1:
            s.virtualHeight = 240.0f;
            s.mode          = DitherMode::RGB;
            s.colorBits     = 5;
            s.ditherSpread  = 1.0f;
            s.vignette      = 0.15f;
            break;

        case DitherStyle::OneBit:
            s.virtualHeight    = 360.0f;
            s.mode             = DitherMode::Luminance;
            s.palette          = {Hex(0x262618), Hex(0xE8F0E0)};
            s.contrast         = 1.35f;
            s.gamma            = 0.85f;
            s.vignette         = 0.35f;
            s.outlineThreshold = 0.35f;
            s.outlineColor     = Hex(0x262618);
            break;

        case DitherStyle::GreenLCD:
            s.virtualHeight = 144.0f;
            s.mode          = DitherMode::Luminance;
            s.palette       = {Hex(0x0F380F), Hex(0x306230), Hex(0x8BAC0F), Hex(0x9BBC0F)};
            s.contrast      = 1.2f;
            s.pixelGrid     = 0.25f;
            break;

        case DitherStyle::Sepia:
            s.virtualHeight    = 270.0f;
            s.mode             = DitherMode::Luminance;
            s.palette          = {Hex(0x1A120B), Hex(0x3C2A1E), Hex(0x7A5A3A), Hex(0xC9A77C), Hex(0xF3E6C8)};
            s.contrast         = 1.15f;
            s.vignette         = 0.5f;
            s.outlineThreshold = 0.45f;
            s.outlineColor     = Hex(0x1A120B);
            break;

        case DitherStyle::Retro16:
            s.virtualHeight = 225.0f;
            s.mode          = DitherMode::Palette;
            s.ditherSpread  = 0.6f;
            s.palette       = {
                Hex(0x0B0B14), Hex(0x2A1F3D), Hex(0x4A2F5C), Hex(0x7A3B5E),
                Hex(0xB4474B), Hex(0xE07A3F), Hex(0xF2C14E), Hex(0xF7F0C6),
                Hex(0x9BD86A), Hex(0x3FA36B), Hex(0x1F6E6A), Hex(0x1D3F5E),
                Hex(0x3B6FB6), Hex(0x6FB3E0), Hex(0x8A8A9A), Hex(0xFFFFFF)
            };
            s.vignette = 0.2f;
            break;
    }

    return s;
}
