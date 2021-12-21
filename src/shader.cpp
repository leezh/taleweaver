#include <stdio.h>
#include <stdlib.h>
#include <SDL_log.h>
#include "shader.hpp"

ShaderCompiler::ShaderCompiler() {
}

ShaderCompiler::~ShaderCompiler() {
    for (Stage &stage : stages) {
        glDeleteShader(stage.shader);
    }
}

void ShaderCompiler::compileStage(GLenum type, const GLchar *source) {
    Stage stage = {glCreateShader(type), source};
    glShaderSource(stage.shader, 1, &source, 0);
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

void ShaderCompiler::bindFragmentOutput(GLuint index, const GLchar *name) {
    Output output = {index, name};
    fragmentOutputs.push_back(output);
}

GLuint ShaderCompiler::create() {
    GLuint program = glCreateProgram();
    for (Stage &stage : stages) {
        glAttachShader(program, stage.shader);
    }
    for (Output &output : fragmentOutputs) {
        glBindFragDataLocation(program, output.index, output.name);
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
