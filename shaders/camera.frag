in vec2 uv;
layout(location=0) out vec4 outColor;
uniform sampler2D tex;

void main() {
    outColor = texture(tex, uv);
}
