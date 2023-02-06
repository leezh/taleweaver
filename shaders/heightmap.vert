layout(location = 1) in vec2 position;

out vec2 uv;
out float height;

uniform mat4 xform;
uniform vec2 scale;
uniform vec2 tex_scale;
uniform vec2 tex_offset;
uniform sampler2D tex;

void main() {
    vec2 p = scale * position;
    uv = p * tex_scale + tex_offset;
    height = texture(tex, uv).r;
    gl_Position = xform * vec4(p.x, height, p.y, 1.0);
}
