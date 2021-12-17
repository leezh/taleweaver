#version 150 core

in vec2 uv;
out vec4 outColor;
uniform vec4 color;
uniform sampler2DRect tex;

void main() {
    outColor = texture(tex, uv) * color;
}
