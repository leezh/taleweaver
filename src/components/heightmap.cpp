#include "heightmap.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <stb_image.h>

static const GLchar vertex[] = {
#include "shaders/heightmap.vert.cstr"
};
static const GLchar fragment[] = {
#include "shaders/heightmap.frag.cstr"
};

Heightmap::~Heightmap() {
    if (texture) glDeleteTextures(1, &texture);
}

bool Heightmap::loadFromImage(const char *path, float min, float max) {
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

bool Heightmap::loadFromMemory(const unsigned char *buffer, int length, float min, float max) {
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
    if (!texture) glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
}

HeightmapSystem::HeightmapSystem(unsigned int detail): detail(detail) {
    unsigned int tileLength = (1 << detail);
    unsigned int stitchLength = tileLength >> 1;

    std::vector<glm::vec2> points;
    points.reserve(tileLength * tileLength + stitchLength * 4);

    std::vector<GLuint> indices;
    unsigned int tileTriangles = (tileLength - 1) * (tileLength - 1) * 2;
    unsigned int stitchTriangles = stitchLength * 2 * 3 - 2;
    indices.reserve((tileTriangles + stitchTriangles) * 3);

    for (int x = 0; x < tileLength; x++) {
        for (int y = 0; y < tileLength; y++) {
            points.emplace_back(glm::vec2((float)x, (float)y) / (float)tileLength);
        }
    }
    for (int x = 0; x < stitchLength; x++) {
        for (int y = 0; y < stitchLength; y++) {
            int p1 = (x + 0) * tileLength + (y + 0);
            int p2 = (x + 1) * tileLength + (y + 0);
            int p3 = (x + 1) * tileLength + (y + 1);
            int p4 = (x + 0) * tileLength + (y + 1);
            indices.emplace_back(p1);
            indices.emplace_back(p2);
            indices.emplace_back(p4);
            indices.emplace_back(p2);
            indices.emplace_back(p3);
            indices.emplace_back(p4);
        }
    }
    for (int x = 0; x < tileLength - 1; x++) {
        for (int y = 0; y < tileLength - 1; y++) {
            if (x < stitchLength && y < stitchLength) continue;
            int p1 = (x + 0) * tileLength + (y + 0);
            int p2 = (x + 1) * tileLength + (y + 0);
            int p3 = (x + 1) * tileLength + (y + 1);
            int p4 = (x + 0) * tileLength + (y + 1);
            indices.emplace_back(p1);
            indices.emplace_back(p2);
            indices.emplace_back(p4);
            indices.emplace_back(p2);
            indices.emplace_back(p3);
            indices.emplace_back(p4);
        }
    }

    for (int j = 0; j < stitchLength; j++) {
        float x = tileLength;
        float y = j * 2;
        points.emplace_back(glm::vec2(x, y) / (float)tileLength);
        points.emplace_back(glm::vec2(x, y + 2) / (float)tileLength);
    }
    for (int j = 0; j < stitchLength; j++) {
        int x = tileLength - 1;
        int y = j * 2;
        int offset = tileLength * tileLength;
        int p1 = (x + 0) * tileLength + (y + 0);
        int p2 = offset + j * 2;
        int p3 = offset + j * 2 + 1;
        int p4 = (x + 0) * tileLength + (y + 1);
        int p5 = (x + 0) * tileLength + (y + 2);
        indices.emplace_back(p1);
        indices.emplace_back(p2);
        indices.emplace_back(p4);
        indices.emplace_back(p2);
        indices.emplace_back(p3);
        indices.emplace_back(p4);
        if (j < stitchLength - 1) {
            indices.emplace_back(p4);
            indices.emplace_back(p3);
            indices.emplace_back(p5);
        }
    }

    for (int j = 0; j < stitchLength; j++) {
        float x = j * 2;
        float y = tileLength;
        points.emplace_back(glm::vec2(x + 2, y) / (float)tileLength);
        points.emplace_back(glm::vec2(x, y) / (float)tileLength);
    }
    for (int j = 0; j < stitchLength; j++) {
        int x = j * 2;
        int y = tileLength - 1;
        int offset = tileLength * tileLength + stitchLength * 2;
        int p1 = (x + 0) * tileLength + (y + 0);
        int p2 = (x + 1) * tileLength + (y + 0);
        int p3 = offset + j * 2;
        int p4 = offset + j * 2 + 1;
        int p5 = (x + 2) * tileLength + (y + 0);
        indices.emplace_back(p1);
        indices.emplace_back(p2);
        indices.emplace_back(p4);
        indices.emplace_back(p2);
        indices.emplace_back(p3);
        indices.emplace_back(p4);
        if (j < stitchLength - 1) {
            indices.emplace_back(p3);
            indices.emplace_back(p2);
            indices.emplace_back(p5);
        }
    }

    indexCount = indices.size();
    stitchIndexOffset = (void*)(stitchLength * stitchLength * 6 * sizeof(GLuint));
    stitchIndexCount = indexCount - stitchLength * stitchLength * 6;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    ShaderCompiler compiler;
    compiler.compileStage(GL_VERTEX_SHADER, vertex);
    compiler.compileStage(GL_FRAGMENT_SHADER, fragment);
    program = compiler.create();

    glUseProgram(program);
    locPosition = glGetAttribLocation(program, "position");
    locTex = glGetUniformLocation(program, "tex");
    locXForm = glGetUniformLocation(program, "xform");
    locScale = glGetUniformLocation(program, "scale");
    locTexOffset = glGetUniformLocation(program, "texOffset");
    locTexScale = glGetUniformLocation(program, "texScale");
    glEnableVertexAttribArray(locPosition);
    glVertexAttribPointer(locPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

HeightmapSystem::~HeightmapSystem() {
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
}

void HeightmapSystem::render(const Heightmap& heightmap, CameraSystem &viewport) {
    float chunkSize = (float)(1 << detail);
    float halfChunk = chunkSize / 2.f;

    auto camera = viewport.get_camera();
    auto center = viewport.get_position();
    center.y = 0.f;
    center.x = glm::round(center.x / halfChunk) * halfChunk;
    center.z = glm::round(center.z / halfChunk) * halfChunk;

    auto size = glm::vec2(heightmap.width, heightmap.height);
    auto texScale = chunkSize / size;
    auto texOffset = glm::vec2(center.x, center.z) / size;
    auto xform = glm::translate(viewport.get_view(), center);
    xform = glm::scale(xform, glm::vec3(chunkSize, 1.f, chunkSize));

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmap.texture);
    glBindVertexArray(vao);
    glUniform1i(locTex, 0);
    glUniform2f(locTexScale, texScale.x, texScale.y);
    glUniform2f(locTexOffset, texOffset.x, texOffset.y);
    glUniformMatrix4fv(locXForm, 1, GL_FALSE, &xform[0][0]);

    unsigned int bufferCount = indexCount;
    void *bufferOffset = 0;
    for (float scale = 1.f; chunkSize * scale / 4 < camera.far; scale *= 2) {
        glUniform2f(locScale, scale, scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        glUniform2f(locScale, scale, -scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        glUniform2f(locScale, -scale, -scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        glUniform2f(locScale, -scale, scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        bufferCount = stitchIndexCount;
        bufferOffset = stitchIndexOffset;
    }
}
