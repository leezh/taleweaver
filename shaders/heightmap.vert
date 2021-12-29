#version 150 core

in vec2 position;
out vec2 uv;
out float height;
uniform mat4 xform;
uniform vec2 scale;
uniform vec2 texScale;
uniform vec2 texOffset;
uniform sampler2D tex;

void main() {
    vec2 p = scale * position;
    uv = p * texScale + texOffset + vec2(0.5, 0.5);
    height = texture(tex, uv).r;
    gl_Position = xform * vec4(p.x, height, p.y, 1.0);
}
