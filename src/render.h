#ifndef TALEWEAVER_RENDER_H
#define TALEWEAVER_RENDER_H

#include <glad/gl.h>

typedef struct _RenderGlyph {
    GLuint tex;
    float w;
    float h;
    float tex_x;
    float tex_y;
    float r;
    float g;
    float b;
    float a;
} RenderGlyph;

void renderInit();
void renderQuit();
void renderInitGlyph(RenderGlyph *glyph);
void renderPrepare(int width, int height);
void renderGlyph(RenderGlyph *glyph, float x, float y);
void renderGlyphCropped(RenderGlyph *glyph, float x, float y, float left, float top, float right, float bottom);

#endif
