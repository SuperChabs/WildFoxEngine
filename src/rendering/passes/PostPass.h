/**
 * @file PostPass.h
 * @brief
 *
 * @author SuperChabs
 * @date 26.11.26
 */

#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "RenderPass.h"

enum class DitherStyle {
    PS1,      // 15-bit color, 240p
    OneBit,   // two tones with ink outlines
    GreenLCD, // 4 shades of green, 144p, visible pixel grid
    Sepia,    // warm 5-tone ramp with outlines
    Retro16   // 16 colors, nearest palette color
};

enum class DitherMode : int {
    RGB       = 0, // per-channel quantize
    Luminance = 1, // brightness picks a color from the palette ramp
    Palette   = 2  // nearest palette color
};

struct DitherSettings {
    float      virtualHeight = 240.0f; // 0 = native resolution
    DitherMode mode          = DitherMode::RGB;
    int        colorBits     = 5;      // RGB mode only
    float      ditherSpread  = 1.0f;   // 0 = no dither

    std::vector<glm::vec3> palette;    // up to 16, dark to light for Luminance mode

    float exposure = 1.0f;
    float contrast = 1.0f;
    float gamma    = 1.0f;
    float vignette = 0.0f;

    float     outlineThreshold = 0.0f; // 0 = off
    glm::vec3 outlineColor{0.0f};

    float pixelGrid = 0.0f;            // 0 = off
};

/**
 * @brief pass for post-processing
 *
 * Reads ctx.sceneColor and writes to ctx.targetFBO with an ordered-dither look.
 */
class PostPass : public RenderPass {
    GLuint m_emptyVAO = 0; // fullscreen triangle is generated from gl_VertexID, but core profile needs a bound VAO

    DitherSettings m_settings;

public:
    static constexpr int MAX_PALETTE = 16;

    /**
     * @brief Default constructor.
     */
    explicit PostPass(ShaderManager *sm);

    ~PostPass() override;

    void Setup() override;
    void Execute(FrameContext &ctx) override;
    void Cleanup() override;

    void SetStyle(DitherStyle style) { m_settings = MakePreset(style); }

    DitherSettings &GetSettings() { return m_settings; }

    static DitherSettings MakePreset(DitherStyle style);
};
