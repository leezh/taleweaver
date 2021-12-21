#include <stdlib.h>
#include <glad/gl.h>
#include "heightmap.h"
#include "shader.h"

static GLuint program = 0;
static const GLchar vertex[] = {
#include "shaders/heightmap.vert.cstr"
};
static const GLchar fragment[] = {
#include "shaders/heightmap.frag.cstr"
};

static GLuint vao = 0;
static GLuint vbo = 0;
static GLuint ebo = 0;
static GLint locPosition;
static GLint locTexOffset;
static GLint locTexScale;

static size_t indexCount = 0;
static size_t borderIndexCount = 0;
static void * borderOffset = 0;

void heightmapInit() {
    size_t power = 3;
    size_t length = (2 << power);
    size_t triCount = (length - 1) * (length - 1) * 2;

    size_t edgeLength = length >> 1;
    size_t edgeIndexOffset = length * length;
    size_t edgeTriCount = edgeLength * 2 * 3 - 2;

    indexCount = (triCount + edgeTriCount) * 3;
    borderIndexCount = indexCount - edgeLength * edgeLength * 6;
    borderOffset = (void *)(edgeLength * edgeLength * 6 * sizeof(GLfloat));

    size_t pointSize = (length * length + edgeLength * 4) * 2 * sizeof(GLfloat);
    GLfloat *points = malloc(pointSize);
    GLfloat *p = points;

    size_t indexSize = indexCount * sizeof(GLuint);
    GLuint *indices = malloc(indexSize);
    GLuint *ci = indices;
    GLuint *i = &indices[edgeLength * edgeLength * 6];

    for (int x = 0; x < length; x++) {
        for (int y = 0; y < length; y++) {
            *(p++) = (GLfloat)(x) / length;
            *(p++) = (GLfloat)(y) / length;
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
        *(p++) = (GLfloat)(x + 1) / length;
        *(p++) = (GLfloat)(y + 0) / length;
        *(p++) = (GLfloat)(x + 1) / length;
        *(p++) = (GLfloat)(y + 2) / length;
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
        *(p++) = (GLfloat)(x + 2) / length;
        *(p++) = (GLfloat)(y + 1) / length;
        *(p++) = (GLfloat)(x + 0) / length;
        *(p++) = (GLfloat)(y + 1) / length;
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

    free(points);
    free(indices);
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    compileShader(vertexShader, vertex);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    compileShader(fragmentShader, fragment);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glBindFragDataLocation(program, 0, "outColor");
    linkProgram(program);

    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(program);
    locPosition = glGetAttribLocation(program, "position");
    locTexOffset = glGetUniformLocation(program, "texOffset");
    locTexScale = glGetUniformLocation(program, "texScale");
    glEnableVertexAttribArray(locPosition);
    glVertexAttribPointer(locPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void heightmapQuit() {
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void heightmapRender() {
    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, borderIndexCount, GL_UNSIGNED_INT, borderOffset);
}
