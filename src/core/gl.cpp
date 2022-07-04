#include "gl.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <SDL_log.h>

#ifndef USE_GLES
static const GLchar* shaderHeader = "#version 330 core\n";
#else
static const GLchar* shaderHeader = "#version 300 es\nprecision mediump float;\n";
#endif

ShaderCompiler::ShaderCompiler() {
}

ShaderCompiler::~ShaderCompiler() {
    for (Stage &stage : stages) {
        glDeleteShader(stage.shader);
    }
}

void ShaderCompiler::compileStage(GLenum type, const GLchar *source) {
    const GLchar* sources[] = {shaderHeader, source};
    Stage stage = {glCreateShader(type), source};
    glShaderSource(stage.shader, 2, sources, 0);
    glCompileShader(stage.shader);
    GLint status;
    glGetShaderiv(stage.shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLsizei length;
        glGetShaderiv(stage.shader, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length + 1];
        log[length] = '\0';
        glGetShaderInfoLog(stage.shader, length, &length, log);
        SDL_LogError(0, "%s\nCode:\n%s", log, source);
        return;
    }
    stages.push_back(stage);
}

GLuint ShaderCompiler::create() {
    GLuint program = glCreateProgram();
    for (Stage &stage : stages) {
        glAttachShader(program, stage.shader);
    }
    glLinkProgram(program);
    for (Stage &stage : stages) {
        glDetachShader(program, stage.shader);
    }
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        GLsizei length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length + 1];
        log[length] = '\0';
        glGetProgramInfoLog(program, length, &length, log);
        SDL_LogError(0, "%s", log);
    }
    return program;
}
