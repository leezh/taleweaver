layout(location=0) out vec4 out_color;

in vec2 uv;

uniform sampler2D tex;

void main() {
    out_color = texture(tex, uv);
}
