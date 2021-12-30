#pragma once
#include <vector>
#include <glad/gl.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

class Heightmap {
    private:
        std::vector<float> data;
        unsigned int detail;
        float chunkSize;
        float mapWidth;
        int pixelWidth;
        int pixelHeight;

        unsigned int power;
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
        bool loadedGL;

        void initGL();
        void quitGL();

    public:
        Heightmap(unsigned int detail, float chunksize);
        ~Heightmap();
        bool loadFromFile(const char *path, float size);
        void upload();
        void render(glm::mat4x4 view, glm::vec3 center, float renderDistance);
};
