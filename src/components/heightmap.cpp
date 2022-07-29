#include "heightmap.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>

static const GLchar vertex_shader[] = {
#include "shaders/heightmap.vert.cstr"
};
static const GLchar fragment_shader[] = {
#include "shaders/heightmap.frag.cstr"
};

Heightmap::~Heightmap() {
    if (init_textures) {
        glDeleteTextures(1, &height_texture);
        glDeleteTextures(1, &normal_texture);
        glDeleteTextures(1, &color_texture);
    }
    if (init_gl) {
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
    }
}

float &Heightmap::height(int x, int y) {
    return height_data[x + y * cols];
}

const float &Heightmap::height(int x, int y) const {
    return height_data[x + y * cols];
}

glm::u8vec4 &Heightmap::color(int x, int y) {
    return color_data[x + y * cols];
}

const glm::u8vec4 &Heightmap::color(int x, int y) const {
    return color_data[x + y * cols];
}

int Heightmap::get_cols() {
    return cols;
}

int Heightmap::get_rows() {
    return rows;
}

void Heightmap::resize(int new_cols, int new_rows) {
    cols = new_cols;
    rows = new_rows;

    height_data.resize(cols * rows);
    normal_data.resize(cols * rows);
    color_data.resize(cols * rows);

    if (init_textures) {
        glDeleteTextures(1, &height_texture);
        glDeleteTextures(1, &normal_texture);
        glDeleteTextures(1, &color_texture);
        init_textures = false;
    }
}

void Heightmap::generate_normals() {
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            //
        }
    }
}

void Heightmap::generate_textures() {
    unsigned int chunk_length = (1 << detail);
    if (!init_gl) {
        unsigned int edge_length = chunk_length >> 1;

        std::vector<glm::vec2> points;
        points.reserve(chunk_length * chunk_length + edge_length * 4);

        std::vector<GLuint> indices;
        unsigned int tileTriangles = (chunk_length - 1) * (chunk_length - 1) * 2;
        unsigned int stitchTriangles = edge_length * 2 * 3 - 2;
        indices.reserve((tileTriangles + stitchTriangles) * 3);

        for (int x = 0; x < chunk_length; x++) {
            for (int y = 0; y < chunk_length; y++) {
                points.emplace_back(glm::vec2((float)x, (float)y) / (float)chunk_length);
            }
        }
        for (int x = 0; x < edge_length; x++) {
            for (int y = 0; y < edge_length; y++) {
                int p1 = (x + 0) * chunk_length + (y + 0);
                int p2 = (x + 1) * chunk_length + (y + 0);
                int p3 = (x + 1) * chunk_length + (y + 1);
                int p4 = (x + 0) * chunk_length + (y + 1);
                indices.emplace_back(p1);
                indices.emplace_back(p2);
                indices.emplace_back(p4);
                indices.emplace_back(p2);
                indices.emplace_back(p3);
                indices.emplace_back(p4);
            }
        }

        stitch_offset = indices.size();

        for (int x = 0; x < chunk_length - 1; x++) {
            for (int y = 0; y < chunk_length - 1; y++) {
                if (x < edge_length && y < edge_length) continue;
                int p1 = (x + 0) * chunk_length + (y + 0);
                int p2 = (x + 1) * chunk_length + (y + 0);
                int p3 = (x + 1) * chunk_length + (y + 1);
                int p4 = (x + 0) * chunk_length + (y + 1);
                indices.emplace_back(p1);
                indices.emplace_back(p2);
                indices.emplace_back(p4);
                indices.emplace_back(p2);
                indices.emplace_back(p3);
                indices.emplace_back(p4);
            }
        }

        for (int j = 0; j < edge_length; j++) {
            float x = chunk_length;
            float y = j * 2;
            points.emplace_back(glm::vec2(x, y) / (float)chunk_length);
            points.emplace_back(glm::vec2(x, y + 2) / (float)chunk_length);
        }
        for (int j = 0; j < edge_length; j++) {
            int x = chunk_length - 1;
            int y = j * 2;
            int offset = chunk_length * chunk_length;
            int p1 = (x + 0) * chunk_length + (y + 0);
            int p2 = offset + j * 2;
            int p3 = offset + j * 2 + 1;
            int p4 = (x + 0) * chunk_length + (y + 1);
            int p5 = (x + 0) * chunk_length + (y + 2);
            indices.emplace_back(p1);
            indices.emplace_back(p2);
            indices.emplace_back(p4);
            indices.emplace_back(p2);
            indices.emplace_back(p3);
            indices.emplace_back(p4);
            if (j < edge_length - 1) {
                indices.emplace_back(p4);
                indices.emplace_back(p3);
                indices.emplace_back(p5);
            }
        }

        for (int j = 0; j < edge_length; j++) {
            float x = j * 2;
            float y = chunk_length;
            points.emplace_back(glm::vec2(x + 2, y) / (float)chunk_length);
            points.emplace_back(glm::vec2(x, y) / (float)chunk_length);
        }
        for (int j = 0; j < edge_length; j++) {
            int x = j * 2;
            int y = chunk_length - 1;
            int offset = chunk_length * chunk_length + edge_length * 2;
            int p1 = (x + 0) * chunk_length + (y + 0);
            int p2 = (x + 1) * chunk_length + (y + 0);
            int p3 = offset + j * 2;
            int p4 = offset + j * 2 + 1;
            int p5 = (x + 2) * chunk_length + (y + 0);
            indices.emplace_back(p1);
            indices.emplace_back(p2);
            indices.emplace_back(p4);
            indices.emplace_back(p2);
            indices.emplace_back(p3);
            indices.emplace_back(p4);
            if (j < edge_length - 1) {
                indices.emplace_back(p3);
                indices.emplace_back(p2);
                indices.emplace_back(p5);
            }
        }

        chunk_size = indices.size();
        stitch_size = chunk_size - stitch_offset;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        ShaderCompiler compiler;
        compiler.compileStage(GL_VERTEX_SHADER, vertex_shader);
        compiler.compileStage(GL_FRAGMENT_SHADER, fragment_shader);
        program = compiler.create();

        glUseProgram(program);
        loc_position = glGetAttribLocation(program, "position");
        loc_height_texture = glGetUniformLocation(program, "height_texture");
        loc_normal_texture = glGetUniformLocation(program, "normal_texture");
        loc_color_texture = glGetUniformLocation(program, "color_texture");
        loc_xform = glGetUniformLocation(program, "xform");
        loc_scale = glGetUniformLocation(program, "scale");
        loc_tex_offset = glGetUniformLocation(program, "tex_offset");
        loc_tex_Scale = glGetUniformLocation(program, "tex_scale");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

        init_gl = true;
    }

    if (init_textures) {
        glDeleteTextures(1, &height_texture);
        glDeleteTextures(1, &normal_texture);
        glDeleteTextures(1, &color_texture);
    }

    glGenTextures(1, &height_texture);
    glBindTexture(GL_TEXTURE_2D, height_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, cols, rows, 0, GL_RED, GL_FLOAT, &height_data[0]);

    glGenTextures(1, &normal_texture);
    glBindTexture(GL_TEXTURE_2D, normal_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, cols, rows, 0, GL_RG, GL_UNSIGNED_BYTE, &normal_data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cols, rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color_data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    init_textures = true;
}

void Heightmap::render(CameraSystem &viewport) {
    unsigned int chunk_length = (1 << detail);
    float half_chunk = .5f * chunk_length;

    auto camera = viewport.get_camera();
    auto center = viewport.get_position();
    center.y = 0.f;
    center.x = glm::round(center.x / half_chunk) * half_chunk;
    center.z = glm::round(center.z / half_chunk) * half_chunk;

    auto size = glm::vec2(cols, rows);
    auto tex_scale = (float)chunk_length / size;
    auto tex_offset = glm::vec2(center.x + .5f, center.z + .5f) / size + glm::vec2(.5f);
    auto xform = glm::translate(viewport.get_view(), center);
    xform = glm::scale(xform, glm::vec3(chunk_length, 1.f, chunk_length));

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, height_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_texture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glBindVertexArray(vao);
    glUniform1i(loc_height_texture, 0);
    glUniform1i(loc_normal_texture, 1);
    glUniform1i(loc_color_texture, 2);
    glUniform2f(loc_tex_Scale, tex_scale.x, tex_scale.y);
    glUniform2f(loc_tex_offset, tex_offset.x, tex_offset.y);
    glUniformMatrix4fv(loc_xform, 1, GL_FALSE, &xform[0][0]);

    unsigned int buffer_size = chunk_size;
    void *buffer_offset = 0;
    for (float scale = 1.f; chunk_length * scale / 4 < camera.far; scale *= 2) {
        glUniform2f(loc_scale, scale, scale);
        glDrawElements(GL_TRIANGLES, buffer_size, GL_UNSIGNED_INT, buffer_offset);

        glUniform2f(loc_scale, scale, -scale);
        glDrawElements(GL_TRIANGLES, buffer_size, GL_UNSIGNED_INT, buffer_offset);

        glUniform2f(loc_scale, -scale, -scale);
        glDrawElements(GL_TRIANGLES, buffer_size, GL_UNSIGNED_INT, buffer_offset);

        glUniform2f(loc_scale, -scale, scale);
        glDrawElements(GL_TRIANGLES, buffer_size, GL_UNSIGNED_INT, buffer_offset);

        buffer_size = stitch_size;
        buffer_offset = (void*)(stitch_offset * sizeof(GLuint));
    }
}
