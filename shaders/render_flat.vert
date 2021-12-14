#version 430

layout(location = 0) in vec2 position;
layout(location = 0) uniform vec2 resolution;

void main() {
    gl_Position = vec4(position / resolution, 0.0, 1.0);
}
