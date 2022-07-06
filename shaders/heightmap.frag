in vec2 uv;
in float height;
layout(location=0) out vec4 outColor;
uniform sampler2D tex;

void main() {
    float h = texture(tex, uv).g;
    h = h * h * 0.9 + 0.1;
    outColor = vec4(vec3(1.0, 1.0, 1.0) * h, 1.0);
}
