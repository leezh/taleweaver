layout(location=0) out vec4 out_color;

in vec2 uv;
in float height;

uniform sampler2D normal_texture;
uniform sampler2D color_texture;

void main() {
    if (uv.x < 0.0) discard;
    if (uv.x > 1.0) discard;
    if (uv.y < 0.0) discard;
    if (uv.y > 1.0) discard;
    out_color = texture(color_texture, uv);
}
