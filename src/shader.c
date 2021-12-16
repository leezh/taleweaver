#include <stdio.h>
#include <stdlib.h>
#include <SDL_log.h>
#include "shader.h"

void checkShader(GLuint shader) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLsizei length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = malloc(length + 1);
        log[length] = '\0';
        glGetShaderInfoLog(shader, length, &length, log);
        SDL_LogError(0, "%s", log);
    }
}
