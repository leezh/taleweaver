#include "heightmap.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>

static const GLchar vertex_shader[] = {
#include "shaders/heightmap.vert.cstr"
};
static const GLchar fragment_shader[] = {
#include "shaders/heightmap.frag.cstr"
};

HeightmapCache::~HeightmapCache() {
    if (height_texture) glDeleteTextures(1, &height_texture);
    if (normal_texture) glDeleteTextures(1, &normal_texture);
    if (color_texture) glDeleteTextures(1, &color_texture);
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
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

int Heightmap::get_cols() const {
    return cols;
}

int Heightmap::get_rows() const {
    return rows;
}

void Heightmap::resize(int new_cols, int new_rows) {
    cols = new_cols;
    rows = new_rows;

    height_data.resize(cols * rows);
    normal_data.resize(cols * rows);
    color_data.resize(cols * rows);
}

void Heightmap::generate_normals() {
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            //
        }
    }
}

HeightmapSystem::HeightmapSystem(entt::registry &registry): registry(registry) {
    ShaderCompiler compiler;
    compiler.compileStage(GL_VERTEX_SHADER, vertex_shader);
    compiler.compileStage(GL_FRAGMENT_SHADER, fragment_shader);
    program = compiler.create();
    init_gl = true;

    glUseProgram(program);
    loc_position = glGetAttribLocation(program, "position");
    loc_height_texture = glGetUniformLocation(program, "height_texture");
    loc_normal_texture = glGetUniformLocation(program, "normal_texture");
    loc_color_texture = glGetUniformLocation(program, "color_texture");
    loc_xform = glGetUniformLocation(program, "xform");
}

HeightmapSystem::~HeightmapSystem() {
    if (init_gl) {
        glDeleteProgram(program);
    }
}

void HeightmapSystem::render(const glm::mat4 &view) {
    for (auto [entity, heightmap] : registry.view<Heightmap>(entt::exclude<HeightmapCache>).each()) {
        HeightmapCache cache;

        std::vector<glm::vec2> points;
        points.reserve(heightmap.rows * heightmap.cols);

        std::vector<GLuint> indices;
        points.reserve((heightmap.rows - 1) * (heightmap.cols - 1) * 6);

        for (int x = 0; x < heightmap.cols; x++) {
            for (int y = 0; y < heightmap.rows; y++) {
                float tx = (float)x / (heightmap.cols - 1);
                float ty = (float)y / (heightmap.rows - 1);
                points.emplace_back(glm::vec2(tx, ty));

                if (x + 1 == heightmap.cols || y + 1 == heightmap.rows) continue;
                int p1 = (x + 0) * heightmap.cols + (y + 0);
                int p2 = (x + 1) * heightmap.cols + (y + 0);
                int p3 = (x + 1) * heightmap.cols + (y + 1);
                int p4 = (x + 0) * heightmap.cols + (y + 1);
                indices.emplace_back(p1);
                indices.emplace_back(p2);
                indices.emplace_back(p4);
                indices.emplace_back(p2);
                indices.emplace_back(p3);
                indices.emplace_back(p4);
            }
        }

        glGenVertexArrays(1, &cache.vao);
        glBindVertexArray(cache.vao);

        glGenBuffers(1, &cache.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, cache.vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

        glGenBuffers(1, &cache.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cache.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glUseProgram(program);
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

        cache.elements = indices.size();

        glGenTextures(1, &cache.height_texture);
        glBindTexture(GL_TEXTURE_2D, cache.height_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, heightmap.cols, heightmap.rows, 0, GL_RED, GL_FLOAT, &heightmap.height_data[0]);

        glGenTextures(1, &cache.normal_texture);
        glBindTexture(GL_TEXTURE_2D, cache.normal_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, heightmap.cols, heightmap.rows, 0, GL_RG, GL_UNSIGNED_BYTE, &heightmap.normal_data[0]);

        glGenTextures(1, &cache.color_texture);
        glBindTexture(GL_TEXTURE_2D, cache.color_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, heightmap.cols, heightmap.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, &heightmap.color_data[0]);

        registry.emplace<HeightmapCache>(entity, std::move(cache));
    }

    for (auto [entity, cache, spatial] : registry.view<HeightmapCache, Spatial>().each()) {
        auto xform = view * spatial.transform;

        glBindVertexArray(cache.vao);
        glBindBuffer(GL_ARRAY_BUFFER, cache.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cache.ebo);

        glUseProgram(program);
        glUniform1i(loc_height_texture, 0);
        glUniform1i(loc_normal_texture, 1);
        glUniform1i(loc_color_texture, 2);
        glUniformMatrix4fv(loc_xform, 1, GL_FALSE, &xform[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cache.height_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cache.normal_texture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cache.color_texture);

        glDrawElements(GL_TRIANGLES, cache.elements, GL_UNSIGNED_INT, 0);
    }
}
