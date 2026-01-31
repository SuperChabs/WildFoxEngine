#version 330 core

out vec4 FragColor;

in vec3 nearPoint;
in vec3 farPoint;

uniform vec3 cameraPos;

uniform float gridScale;
uniform float gridFadeDistance;
uniform float gridFadeStart;

uniform vec3 gridColorThin;
uniform vec3 gridColorThick;
uniform vec3 axisXColor;
uniform vec3 axisZColor;

uniform float lineThickness;

float ComputeDepth(vec3 pos, mat4 projection, mat4 view)
{
    vec4 clipSpacePos = projection * view * vec4(pos, 1.0);
    float ndcDepth = clipSpacePos.z / clipSpacePos.w;
    return ((ndcDepth + 1.0) * 0.5);
}

vec4 Grid(vec3 fragPos, float scale)
{
    vec2 coord = fragPos.xz / scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);

    float minZ = min(derivative.y, 1.0);
    float minX = min(derivative.x, 1.0);

    vec4 color = vec4(gridColorThin, 1.0 - min(line, 1.0));

    if (abs(mod(coord.x, 10.0)) < minX * lineThickness ||
        abs(mod(coord.y, 10.0)) < minZ * lineThickness)
    {
        color = vec4(gridColorThick, 1.0);
    }

    float axisWidth = lineThickness * 2.0;
    if(abs(fragPos.x) < minX * lineThickness * axisWidth) 
        color = vec4(axisZColor, 1.0);
    if(abs(fragPos.z) < minZ * lineThickness * axisWidth) 
        color = vec4(axisXColor, 1.0);
    
    return color;
}

void main() 
{
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    
    if(t < 0.0) 
        discard;
    
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    
    gl_FragDepth = ComputeDepth(fragPos3D, mat4(1.0), mat4(1.0));
    
    float distanceFromCamera = length(cameraPos - fragPos3D);
    
    float fadeFactor = 1.0 - smoothstep(gridFadeStart, gridFadeDistance, distanceFromCamera);
    
    vec4 gridColor = Grid(fragPos3D, gridScale);
    
    gridColor.a *= fadeFactor;
    
    if(gridColor.a < 0.01) 
        discard;
    
    FragColor = gridColor;
}