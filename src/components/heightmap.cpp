#include "heightmap.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <stb_image.h>

static const GLchar vertex_shader[] = {
#include "shaders/heightmap.vert.cstr"
};
static const GLchar fragment_shader[] = {
#include "shaders/heightmap.frag.cstr"
};

Heightmap::~Heightmap() {
    if (texture) glDeleteTextures(1, &texture);
}

glm::vec3 &Heightmap::at(int x, int y) {
    return data[x + y * width];
}

int Heightmap::get_width() {
    return width;
}

int Heightmap::get_height() {
    return height;
}

void Heightmap::resize(int new_width, int new_height) {
    width = new_width;
    height = new_height;
    data.resize(width * height);
}

bool Heightmap::load_image_file(const char *path, float min, float max) {
    int n;
    auto image = stbi_load(path, &width, &height, &n, 3);
    if (image == nullptr) {
        return false;
    }
    data.resize(width * height);
    auto pixel = &image[0];
    for (auto &i : data) {
        i.r = (float)(*pixel++) / 255.f  * (max - min) + min;
        i.g = (float)(*pixel++) / 255.f;
        i.b = (float)(*pixel++) / 255.f;
    }
    stbi_image_free(image);
    upload();
    return true;
}

bool Heightmap::load_image_buffer(const unsigned char *buffer, int length, float min, float max) {
    int n;
    auto image = stbi_load_from_memory(buffer, length, &width, &height, &n, 3);
    if (image == nullptr) {
        return false;
    }
    data.resize(width * height);
    auto pixel = &image[0];
    for (auto &i : data) {
        i.r = (float)(*pixel++) / 255.f  * (max - min) + min;
        i.g = (float)(*pixel++) / 255.f;
        i.b = (float)(*pixel++) / 255.f;
    }
    stbi_image_free(image);
    upload();
    return true;
}

void Heightmap::upload() {
    if (texture) glDeleteTextures(1, &texture);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, &data[0]);
}

HeightmapSystem::HeightmapSystem(unsigned int detail) {
    chunk_length = (1 << detail);
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
    loc_tex = glGetUniformLocation(program, "tex");
    loc_xform = glGetUniformLocation(program, "xform");
    loc_scale = glGetUniformLocation(program, "scale");
    loc_tex_offset = glGetUniformLocation(program, "tex_offset");
    loc_tex_Scale = glGetUniformLocation(program, "tex_scale");
    glEnableVertexAttribArray(loc_position);
    glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

HeightmapSystem::~HeightmapSystem() {
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
}

void HeightmapSystem::render(const Heightmap& heightmap, CameraSystem &viewport) {
    float half_chunk = chunk_length / 2.f;

    auto camera = viewport.get_camera();
    auto center = viewport.get_position();
    center.y = 0.f;
    center.x = glm::round(center.x / half_chunk) * half_chunk;
    center.z = glm::round(center.z / half_chunk) * half_chunk;

    auto size = glm::vec2(heightmap.width, heightmap.height);
    auto tex_scale = (float)chunk_length / size;
    auto tex_offset = glm::vec2(center.x + .5f, center.z + .5f) / size + glm::vec2(.5f);
    auto xform = glm::translate(viewport.get_view(), center);
    xform = glm::scale(xform, glm::vec3(chunk_length, 1.f, chunk_length));

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmap.texture);
    glBindVertexArray(vao);
    glUniform1i(loc_tex, 0);
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
