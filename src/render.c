#include <stdio.h>
#include <glad/gl.h>
#include "render.h"

#define SOURCE_NUM 2

static const GLchar *shaderSources[SOURCE_NUM] = {
#include "shaders/render_flat.vert.cstr"
#include "shaders/render_flat.frag.cstr"
};
static GLuint shaderIds[SOURCE_NUM];
