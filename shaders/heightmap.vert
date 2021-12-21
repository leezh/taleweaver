#version 150 core

in vec2 position;
out vec2 uv;
uniform vec2 texOffset;
uniform vec2 texScale;

void main() {
    uv = position * texScale + texOffset;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
