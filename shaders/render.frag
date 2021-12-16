#version 150 core

in vec2 uv;
out vec4 outColor;
uniform vec4 color;
uniform sampler2D tex;

void main() {
    outColor = vec4(texture(tex, uv).rg, 1.0, 1.0) * color;
}
