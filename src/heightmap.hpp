#pragma once
#include <vector>
#include <glad/gl.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

class Heightmap {
    private:
        std::vector<float> data;
        int width;
        int height;

        bool loadedGL;
        unsigned int detail;
        GLuint texture;
        GLuint program;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLint locPosition;
        GLint locTex;
        GLint locTexOffset;
        GLint locTexScale;
        GLint locScale;
        GLint locOffset;
        GLint locXForm;
        unsigned int indexCount;
        unsigned int borderIndexCount;
        void *borderOffset;

        void initGL();
        void quitGL();

    public:
        Heightmap(unsigned int detail = 7);
        ~Heightmap();
        bool loadFromImage(const char *path, float min, float max);
        bool loadFromMemory(const unsigned char *buffer, int length, float min, float max);
        void upload();
        void render(glm::mat4x4 view, glm::vec3 center, float renderDistance);
};
