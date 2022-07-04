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
    data.resize(width * height * 3, 0);
    auto p = &image[0];
    for (auto &i : data) {
        i = (float)*p / 255.f;
        i = i * i * (max - min) + min;
        p = &p[1];
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
    data.resize(width * height * 3, 0);
    auto p = &image[0];
    for (auto &i : data) {
        i = (float)*p / 255.f;
        i = i * i * (max - min) + min;
        p = &p[1];
    }
    stbi_image_free(image);
    upload();
    return true;
}

void Heightmap::upload() {
    if (!texture) glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RGB, GL_FLOAT, &data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
}

HeightmapSystem::HeightmapSystem(unsigned int detail): detail(detail) {
    unsigned int length = (1 << detail);
    unsigned int triCount = (length - 1) * (length - 1) * 2;

    unsigned int edgeLength = length >> 1;
    unsigned int edgeIndexOffset = length * length;
    unsigned int edgeTriCount = edgeLength * 2 * 3 - 2;

    indexCount = (triCount + edgeTriCount) * 3;
    borderIndexCount = indexCount - edgeLength * edgeLength * 6;
    borderOffset = (void *)(edgeLength * edgeLength * 6 * sizeof(GLfloat));

    unsigned int pointSize = (length * length + edgeLength * 4) * 2 * sizeof(GLfloat);
    GLfloat points[pointSize];
    GLfloat *p = points;

    unsigned int indexSize = indexCount * sizeof(GLuint);
    GLuint indices[indexSize];
    GLuint *ci = indices;
    GLuint *i = &indices[edgeLength * edgeLength * 6];

    for (int x = 0; x < length; x++) {
        for (int y = 0; y < length; y++) {
            *(p++) = (GLfloat)(x) / length / 2;
            *(p++) = (GLfloat)(y) / length / 2;
            int p1 = (x + 0) * length + (y + 0);
            int p2 = (x + 1) * length + (y + 0);
            int p3 = (x + 1) * length + (y + 1);
            int p4 = (x + 0) * length + (y + 1);
            if (x < edgeLength && y < edgeLength) {
                *(ci++) = p1;
                *(ci++) = p2;
                *(ci++) = p4;
                *(ci++) = p2;
                *(ci++) = p3;
                *(ci++) = p4;
            } else if (x < length - 1 && y < length - 1) {
                *(i++) = p1;
                *(i++) = p2;
                *(i++) = p4;
                *(i++) = p2;
                *(i++) = p3;
                *(i++) = p4;
            }
        }
    }

    for (int j = 0; j < edgeLength; j++) {
        int x = length - 1;
        int y = j * 2;
        *(p++) = (GLfloat)(x + 1) / length / 2;
        *(p++) = (GLfloat)(y + 0) / length / 2;
        *(p++) = (GLfloat)(x + 1) / length / 2;
        *(p++) = (GLfloat)(y + 2) / length / 2;
        int p1 = (x + 0) * length + (y + 0);
        int p2 = edgeIndexOffset + j * 2;
        int p3 = edgeIndexOffset + j * 2 + 1;
        int p4 = (x + 0) * length + (y + 1);
        int p5 = (x + 0) * length + (y + 2);
        *(i++) = p1;
        *(i++) = p2;
        *(i++) = p4;
        *(i++) = p2;
        *(i++) = p3;
        *(i++) = p4;
        if (j < edgeLength - 1) {
            *(i++) = p4;
            *(i++) = p3;
            *(i++) = p5;
        }
    }
    for (int j = 0; j < edgeLength; j++) {
        int x = j * 2;
        int y = length - 1;
        *(p++) = (GLfloat)(x + 2) / length / 2;
        *(p++) = (GLfloat)(y + 1) / length / 2;
        *(p++) = (GLfloat)(x + 0) / length / 2;
        *(p++) = (GLfloat)(y + 1) / length / 2;
        int p1 = (x + 0) * length + (y + 0);
        int p2 = (x + 1) * length + (y + 0);
        int p3 = edgeIndexOffset + edgeLength * 2 + j * 2;
        int p4 = edgeIndexOffset + edgeLength * 2 + j * 2 + 1;
        int p5 = (x + 2) * length + (y + 0);
        *(i++) = p1;
        *(i++) = p2;
        *(i++) = p4;
        *(i++) = p2;
        *(i++) = p3;
        *(i++) = p4;
        if (j < edgeLength - 1) {
            *(i++) = p3;
            *(i++) = p2;
            *(i++) = p5;
        }
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, pointSize, points, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);

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

void HeightmapSystem::render(const Heightmap& heightmap, glm::mat4x4 view, glm::vec3 center, float renderDistance) {
    float chunkSize = (float)(1 << detail);
    float halfChunk = chunkSize / 2.f;
    center.y = 0.f;
    center.x = glm::round(center.x / halfChunk) * halfChunk;
    center.z = glm::round(center.z / halfChunk) * halfChunk;
    auto size = glm::vec2(heightmap.width, heightmap.height);
    auto texScale = chunkSize / size;
    auto texOffset = glm::vec2(center.x, center.z) / size;
    auto xform = glm::translate(view, center);
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
    GLfloat scale = 1.f;
    float dist = chunkSize / 4.f;
    while (dist < renderDistance) {
        glUniform2f(locScale, scale, scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        glUniform2f(locScale, scale, -scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        glUniform2f(locScale, -scale, -scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        glUniform2f(locScale, -scale, scale);
        glDrawElements(GL_TRIANGLES, bufferCount, GL_UNSIGNED_INT, bufferOffset);

        bufferCount = borderIndexCount;
        bufferOffset = borderOffset;
        dist *= 2;
        scale *= 2.f;
    }
}
