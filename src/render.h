#ifndef TALEWEAVER_RENDER_H
#define TALEWEAVER_RENDER_H

#include <glad/gl.h>

typedef struct {
    int x;
    int y;
    int w;
    int h;
    GLuint tex;
    float tex_x;
    float tex_y;
    float tex_w;
    float tex_h;
    int r;
    int g;
    int b;
    int a;
} RenderGlyph;

void renderInit();
void renderQuit();
void renderInitGlyph(RenderGlyph *glyph);
void renderPrepare(int width, int height);
void renderGlyph(RenderGlyph *glyph);

#endif
