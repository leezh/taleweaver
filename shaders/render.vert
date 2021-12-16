#version 150 core

in vec2 position;
out vec2 uv;
uniform vec2 resolution;
uniform vec2 offset;
uniform vec2 size;
uniform vec2 texOffset;
uniform vec2 texSize;

void main() {
    float x = (offset.x + position.x * size.x) * 2.0 / resolution.x - 1.0;
    float y = 1.0 - (offset.y + position.y * size.y) * 2.0 / resolution.y;
    gl_Position = vec4(x, y, 0.0, 1.0);
    uv = vec2(texOffset + position * texSize);
}
