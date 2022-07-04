#pragma once
#include <vector>

#ifndef USE_GLES
#include <glad/gl.h>
#else
#include <glad/gles2.h>
#endif

class ShaderCompiler {
    private:
        struct Stage {
            GLenum shader;
            const GLchar *source;
        };
        std::vector<Stage> stages;
    public:
        ShaderCompiler();
        ~ShaderCompiler();
        void compileStage(GLenum type, const GLchar *source);
        GLuint create();
};
