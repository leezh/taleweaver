#ifndef TALEWEAVER_SHADER_H
#define TALEWEAVER_SHADER_H

#include <vector>
#include <glad/gl.h>

class ShaderCompiler {
    private:
        struct Stage {
            GLenum shader;
            const GLchar *source;
        };
        struct Output {
            GLuint index;
            const GLchar *name;
        };
        std::vector<Stage> stages;
        std::vector<Output> fragmentOutputs;
    public:
        ShaderCompiler();
        ~ShaderCompiler();
        void compileStage(GLenum type, const GLchar *source);
        void bindFragmentOutput(GLuint index, const GLchar *name);
        GLuint create();
};

#endif
