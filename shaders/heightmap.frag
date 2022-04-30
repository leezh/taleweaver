#version 150 core

in vec2 uv;
in float height;
out vec4 outColor;
uniform sampler2D tex;

void main() {
    float h = textureLod(tex, uv, 6).r / 50.0;
    h = h * h * 0.95 + 0.05;
    outColor = vec4(vec3(1.0, 1.0, 1.0) * h, 1.0);
}
