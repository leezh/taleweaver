#pragma once
#include <vector>
#include "gl.hpp"

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
