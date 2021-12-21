#ifndef TALEWEAVER_SHADER_H
#define TALEWEAVER_SHADER_H

#include <glad/gl.h>

int compileShader(GLuint shader, const GLchar *source);
int linkProgram(GLuint program);

#endif
