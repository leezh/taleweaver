#version 430

layout(location = 0) out vec4 outColor;
layout(location = 3) uniform vec4 color;

void main() {
    outColor = color;
}
