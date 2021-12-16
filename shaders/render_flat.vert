#version 430

layout(location = 0) in vec2 position;
layout(location = 0) uniform vec2 resolution;
layout(location = 1) uniform vec2 offset;
layout(location = 2) uniform vec2 size;

void main() {
    gl_Position = vec4((offset + position * size) / resolution, 0.0, 1.0);
}
