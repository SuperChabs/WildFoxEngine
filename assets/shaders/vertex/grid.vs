#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 nearPoint;
out vec3 farPoint;

uniform mat4 projection;
uniform mat4 view;

vec3 UnprojectPoint(float x, float y, float z, mat4 viewProj)
{
    mat4 viewProjInv = inverse(viewProj);
    vec4 unprojectedPoint = viewProjInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    mat4 viewProj = projection * view;

    vec3 p = aPos;

    nearPoint = UnprojectPoint(p.x, p.z, 0.0, viewProj).xyz;
    farPoint  = UnprojectPoint(p.x, p.z, 1.0, viewProj).xyz;

    gl_Position = vec4(p.x, p.y, 0.0, 1.0);
}