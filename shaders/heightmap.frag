layout(location=0) out vec4 out_color;

in vec2 uv;
in float height;

uniform sampler2D height_texture;

void main() {
    float h = texture(height_texture, uv).r;
    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1) {
        discard;
    } else if (h > 60) {
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    } else if (h > 2) {
        out_color = mix(vec4(0.3, 0.5, 0.2, 1.0), vec4(0.8, 1.0, 0.8, 1.0), h / 60);
    } else if (h > 0) {
        out_color = vec4(1.0, 1.0, 0.7, 1.0);
    } else {
        out_color = mix(vec4(0.1, 0.7, 0.8, 1.0), vec4(0.0, 0.0, 0.5, 1.0), -h / 100);
    }
}
