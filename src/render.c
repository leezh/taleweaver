#include "render.h"
#include "shader.h"

static GLuint renderProgram = 0;
static const GLchar renderVertex[] = {
#include "shaders/render.vert.cstr"
};
static const GLchar renderFragment[] = {
#include "shaders/render.frag.cstr"
};

static GLint renderLocPosition;
static GLint renderLocResolution;
static GLint renderLocOffset;
static GLint renderLocSize;
static GLint renderLocTexOffset;
static GLint renderLocTex;
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

static const GLubyte renderBlankPixels[] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
};
static GLuint renderBlankTex = 0;

void renderInit() {
    glGenVertexArrays(1, &renderVAO);
    glBindVertexArray(renderVAO);

    glGenBuffers(1, &renderVBO);
    glBindBuffer(GL_ARRAY_BUFFER, renderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(renderVertices), renderVertices, GL_STATIC_DRAW);
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    compileShader(vertexShader, renderVertex);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    compileShader(fragmentShader, renderFragment);

    renderProgram = glCreateProgram();
    glAttachShader(renderProgram, vertexShader);
    glAttachShader(renderProgram, fragmentShader);
    glBindFragDataLocation(renderProgram, 0, "outColor");
    linkProgram(renderProgram);

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
    renderLocTex = glGetUniformLocation(renderProgram, "tex");
    renderLocColor = glGetUniformLocation(renderProgram, "color");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(renderLocPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenTextures(1, &renderBlankTex);
    glBindTexture(GL_TEXTURE_RECTANGLE, renderBlankTex);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderBlankPixels);
}

void renderQuit() {
    glDeleteTextures(1, &renderBlankTex);
    glDeleteProgram(renderProgram);
    glDeleteBuffers(1, &renderVBO);
    glDeleteVertexArrays(1, &renderVAO);
}

void renderInitGlyph(RenderGlyph *glyph) {
    glyph->tex = 0;
    glyph->w = 0.f;
    glyph->h = 0.f;
    glyph->tex_x = 0.f;
    glyph->tex_y = 0.f;
    glyph->r = 1.f;
    glyph->g = 1.f;
    glyph->b = 1.f;
    glyph->a = 1.f;
}

void renderPrepare(int width, int height) {
    glUseProgram(renderProgram);
    glBindVertexArray(renderVAO);
    glUniform2f(renderLocResolution, width, height);
    glUniform1i(renderLocTex, 0);
}

void renderGlyph(RenderGlyph *glyph, float x, float y) {
    glActiveTexture(GL_TEXTURE0);
    if (glyph->tex) {
        glBindTexture(GL_TEXTURE_RECTANGLE, glyph->tex);
    } else {
        glBindTexture(GL_TEXTURE_RECTANGLE, renderBlankTex);
    }
    glUniform2f(renderLocOffset, x, y);
    glUniform2f(renderLocSize, glyph->w, glyph->h);
    glUniform2f(renderLocTexOffset, glyph->tex_x, glyph->tex_y);
    glUniform4f(renderLocColor, glyph->r, glyph->g, glyph->b, glyph->a);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderGlyphCropped(RenderGlyph *glyph, float x, float y, float left, float top, float right, float bottom) {
    glActiveTexture(GL_TEXTURE0);
    if (glyph->tex) {
        glBindTexture(GL_TEXTURE_RECTANGLE, glyph->tex);
    } else {
        glBindTexture(GL_TEXTURE_RECTANGLE, renderBlankTex);
    }
    glUniform2f(renderLocOffset, x, y);
    glUniform2f(renderLocSize, glyph->w - left - right, glyph->h - top - bottom);
    glUniform2f(renderLocTexOffset, glyph->tex_x + left, glyph->tex_y + top);
    glUniform4f(renderLocColor, glyph->r, glyph->g, glyph->b, glyph->a);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
