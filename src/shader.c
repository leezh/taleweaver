#include <stdio.h>
#include <stdlib.h>
#include <SDL_log.h>
#include "shader.h"

int compileShader(GLuint shader, const GLchar *source) {
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLsizei length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = malloc(length + 1);
        log[length] = '\0';
        glGetShaderInfoLog(shader, length, &length, log);
        SDL_LogError(0, "%s\nCode:\n%s", log, source);
        free(log);
        return -1;
    }
    return 0;
}

int linkProgram(GLuint program) {
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLsizei length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar *log = malloc(length + 1);
        log[length] = '\0';
        glGetProgramInfoLog(program, length, &length, log);
        SDL_LogError(0, "%s", log);
        free(log);
        return -1;
    }
    return 0;
}
