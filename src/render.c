#include "render.h"
#include "shader.h"

static GLuint program = 0;
static const GLchar vertex[] = {
#include "shaders/render.vert.cstr"
};
static const GLchar fragment[] = {
#include "shaders/render.frag.cstr"
};

static GLint locPosition;
static GLint locResolution;
static GLint locOffset;
static GLint locSize;
static GLint locTexOffset;
static GLint locTex;
static GLint locColor;

static const GLfloat vertices[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};
static GLuint vbo = 0;
static GLuint vao = 0;

static const GLubyte blankpixels[] = {
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
};
static GLuint blanktex = 0;

void renderInit() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
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
    locResolution = glGetUniformLocation(program, "resolution");
    locOffset = glGetUniformLocation(program, "offset");
    locSize = glGetUniformLocation(program, "size");
    locTexOffset = glGetUniformLocation(program, "texOffset");
    locTex = glGetUniformLocation(program, "tex");
    locColor = glGetUniformLocation(program, "color");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(locPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenTextures(1, &blanktex);
    glBindTexture(GL_TEXTURE_RECTANGLE, blanktex);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, blankpixels);
}

void renderQuit() {
    glDeleteTextures(1, &blanktex);
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
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
    glUseProgram(program);
    glBindVertexArray(vao);
    glUniform2f(locResolution, width, height);
    glUniform1i(locTex, 0);
}

void renderGlyph(RenderGlyph *glyph, float x, float y) {
    glActiveTexture(GL_TEXTURE0);
    if (glyph->tex) {
        glBindTexture(GL_TEXTURE_RECTANGLE, glyph->tex);
    } else {
        glBindTexture(GL_TEXTURE_RECTANGLE, blanktex);
    }
    glUniform2f(locOffset, x, y);
    glUniform2f(locSize, glyph->w, glyph->h);
    glUniform2f(locTexOffset, glyph->tex_x, glyph->tex_y);
    glUniform4f(locColor, glyph->r, glyph->g, glyph->b, glyph->a);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderGlyphCropped(RenderGlyph *glyph, float x, float y, float left, float top, float right, float bottom) {
    glActiveTexture(GL_TEXTURE0);
    if (glyph->tex) {
        glBindTexture(GL_TEXTURE_RECTANGLE, glyph->tex);
    } else {
        glBindTexture(GL_TEXTURE_RECTANGLE, blanktex);
    }
    glUniform2f(locOffset, x, y);
    glUniform2f(locSize, glyph->w - left - right, glyph->h - top - bottom);
    glUniform2f(locTexOffset, glyph->tex_x + left, glyph->tex_y + top);
    glUniform4f(locColor, glyph->r, glyph->g, glyph->b, glyph->a);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
