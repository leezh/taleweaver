#include "render.h"
#include "shader.h"

static GLuint renderProgram = 0;
static const GLchar *renderVertex = {
#include "shaders/render.vert.cstr"
};
static const GLchar *renderFragment = {
#include "shaders/render.frag.cstr"
};

static GLint renderLocPosition;
static GLint renderLocResolution;
static GLint renderLocOffset;
static GLint renderLocSize;
static GLint renderLocTexOffset;
static GLint renderLocTexSize;
static GLint renderLocColor;

static const GLfloat renderVertices[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};
static GLuint renderVBO = 0;
static GLuint renderVAO = 0;

void renderInit() {
    glGenVertexArrays(1, &renderVAO);
    glBindVertexArray(renderVAO);

    glGenBuffers(1, &renderVBO);
    glBindBuffer(GL_ARRAY_BUFFER, renderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(renderVertices), renderVertices, GL_STATIC_DRAW);
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &renderVertex, 0);
    glCompileShader(vertexShader);
    checkShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &renderFragment, 0);
    glCompileShader(fragmentShader);
    checkShader(fragmentShader);

    renderProgram = glCreateProgram();
    glAttachShader(renderProgram, vertexShader);
    glAttachShader(renderProgram, fragmentShader);
    glBindFragDataLocation(renderProgram, 0, "outColor");
    glLinkProgram(renderProgram);

    glDetachShader(renderProgram, vertexShader);
    glDetachShader(renderProgram, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(renderProgram);
    renderLocPosition = glGetAttribLocation(renderProgram, "position");
    renderLocResolution = glGetUniformLocation(renderProgram, "resolution");
    renderLocOffset = glGetUniformLocation(renderProgram, "offset");
    renderLocSize = glGetUniformLocation(renderProgram, "size");
    renderLocTexOffset = glGetUniformLocation(renderProgram, "texOffset");
    renderLocTexSize = glGetUniformLocation(renderProgram, "texSize");
    renderLocColor = glGetUniformLocation(renderProgram, "color");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(renderLocPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void renderQuit() {
    glDeleteProgram(renderProgram);
}

void renderInitGlyph(RenderGlyph *glyph) {
    glyph->x = 0;
    glyph->y = 0;
    glyph->w = 0;
    glyph->h = 0;
    glyph->tex = 0;
    glyph->tex_x = 0;
    glyph->tex_y = 0;
    glyph->tex_w = 0;
    glyph->tex_h = 0;
    glyph->r = 1.f;
    glyph->g = 1.f;
    glyph->b = 1.f;
    glyph->a = 1.f;
}

void renderPrepare(int width, int height) {
    glUseProgram(renderProgram);
    glUniform2f(renderLocResolution, width, height);
}

void renderGlyph(RenderGlyph *glyph) {
    glUniform2f(renderLocOffset, glyph->x, glyph->y);
    glUniform2f(renderLocSize, glyph->w, glyph->h);
    glUniform2f(renderLocTexOffset, glyph->tex_x, glyph->tex_y);
    glUniform2f(renderLocTexSize, glyph->tex_w, glyph->tex_h);
    glUniform4f(renderLocColor, glyph->r, glyph->g, glyph->b, glyph->a);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
