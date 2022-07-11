in vec2 position;
out vec2 uv;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    uv = (position + vec2(1.0, 1.0)) / 2.0;
}
