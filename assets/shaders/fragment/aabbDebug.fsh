#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform float alpha;
uniform int roundPoint;

void main() {
    if (roundPoint == 1) {
        vec2 c = gl_PointCoord * 2.0 - 1.0;
        float d = dot(c, c);
        if (d > 1.0) discard;

        FragColor = vec4(color * mix(1.0, 0.55, d), alpha);
    } else {
        FragColor = vec4(color, alpha);
    }
}