#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneColor;
uniform vec2 screenSize;      // real target size in pixels

// --- look ---
uniform float virtualHeight;  // low "console" vertical resolution, 0 = native
uniform int   mode;           // 0 = per-channel RGB quantize, 1 = luminance ramp, 2 = nearest palette color
uniform float colorLevels;    // mode 0: levels per channel - 1 (31 = 15-bit)
uniform float ditherSpread;   // 0 = no dither, 1 = one quantization step

// --- palette (modes 1 and 2), ordered from dark to light for mode 1 ---
#define MAX_PALETTE 16
uniform vec3 palette[MAX_PALETTE];
uniform int  paletteSize;

// --- tone, applied before dithering so it dissolves into the pattern ---
uniform float exposure;
uniform float contrast;
uniform float gamma;
uniform float vignette;       // 0 = off

// --- ink outline from luminance edges ---
uniform float outlineThreshold; // 0 = off
uniform vec3  outlineColor;

// --- display ---
uniform float pixelGrid;      // 0 = off, darkens the borders of virtual pixels

float luma(vec3 c) { return dot(c, vec3(0.299, 0.587, 0.114)); }

// 8x8 Bayer threshold in [-0.5, 0.5), built from bit interleaving
float bayer8(ivec2 p)
{
    int x = p.x & 7;
    int y = p.y & 7;
    int xy = x ^ y;
    int v = ((xy & 1) << 5) | ((x & 1) << 4)
          | ((xy & 2) << 2) | ((x & 2) << 1)
          | ((xy & 4) >> 1) | ((x & 4) >> 2);
    return (float(v) + 0.5) / 64.0 - 0.5;
}

vec3 fetchVirtual(vec2 virtualPx, float pixelSize)
{
    vec2 px = clamp((virtualPx + 0.5) * pixelSize, vec2(0.0), screenSize - 1.0);
    return texelFetch(sceneColor, ivec2(px), 0).rgb;
}

vec3 tone(vec3 c, vec2 uv)
{
    c *= exposure;
    c = (c - 0.5) * contrast + 0.5;
    c = pow(clamp(c, 0.0, 1.0), vec3(gamma));

    vec2 d = uv - 0.5;
    c *= 1.0 - vignette * dot(d, d) * 2.0;
    return clamp(c, 0.0, 1.0);
}

vec3 nearestPalette(vec3 c)
{
    vec3 best = palette[0];
    float bestDist = 1e9;
    for (int i = 0; i < MAX_PALETTE; i++) {
        if (i >= paletteSize) break;
        vec3 diff = c - palette[i];
        // weight green higher, roughly how the eye judges brightness
        float dist = dot(diff * diff, vec3(0.30, 0.59, 0.11));
        if (dist < bestDist) { bestDist = dist; best = palette[i]; }
    }
    return best;
}

void main()
{
    float pixelSize = virtualHeight > 0.0 ? max(screenSize.y / virtualHeight, 1.0) : 1.0;

    vec2 fragPx    = TexCoords * screenSize;
    vec2 virtualPx = floor(fragPx / pixelSize);
    vec2 virtualUV = (virtualPx + 0.5) * pixelSize / screenSize;

    vec3 color = tone(fetchVirtual(virtualPx, pixelSize), virtualUV);
    float threshold = bayer8(ivec2(virtualPx)) * ditherSpread;

    vec3 result;
    if (mode == 0) {
        result = floor(color * colorLevels + 0.5 + threshold) / colorLevels;
    } else if (mode == 1) {
        float steps = float(max(paletteSize - 1, 1));
        int idx = int(clamp(floor(luma(color) * steps + 0.5 + threshold), 0.0, steps));
        result = palette[idx];
    } else {
        // spread is relative to the average gap between palette colors
        float gap = 1.0 / float(max(paletteSize - 1, 1));
        result = nearestPalette(color + threshold * gap);
    }

    if (outlineThreshold > 0.0) {
        // Sobel on luminance of neighbouring virtual pixels
        float tl = luma(fetchVirtual(virtualPx + vec2(-1,  1), pixelSize));
        float t  = luma(fetchVirtual(virtualPx + vec2( 0,  1), pixelSize));
        float tr = luma(fetchVirtual(virtualPx + vec2( 1,  1), pixelSize));
        float l  = luma(fetchVirtual(virtualPx + vec2(-1,  0), pixelSize));
        float r  = luma(fetchVirtual(virtualPx + vec2( 1,  0), pixelSize));
        float bl = luma(fetchVirtual(virtualPx + vec2(-1, -1), pixelSize));
        float b  = luma(fetchVirtual(virtualPx + vec2( 0, -1), pixelSize));
        float br = luma(fetchVirtual(virtualPx + vec2( 1, -1), pixelSize));

        float gx = (tr + 2.0 * r + br) - (tl + 2.0 * l + bl);
        float gy = (tl + 2.0 * t + tr) - (bl + 2.0 * b + br);

        if (length(vec2(gx, gy)) > outlineThreshold)
            result = outlineColor;
    }

    if (pixelGrid > 0.0 && pixelSize >= 3.0) {
        vec2 inPixel = fract(fragPx / pixelSize);
        float edge = step(inPixel.x, 1.0 / pixelSize) + step(inPixel.y, 1.0 / pixelSize);
        result *= 1.0 - pixelGrid * min(edge, 1.0);
    }

    FragColor = vec4(result, 1.0);
}
