#include "render.h"
#include "shader.h"

static GLuint renderFlatProgram = 0;
static const GLchar renderFlatVertex[] = {
#include "shaders/render_flat.vert.cstr"
};
static const GLchar renderFlatFragment[] = {
#include "shaders/render_flat.frag.cstr"
};


void renderInit() {
    renderFlatProgram = shaderCompileVF(renderFlatVertex, renderFlatFragment);
}

void renderQuit() {
    glDeleteProgram(renderFlatProgram);
}
