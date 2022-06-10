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
        float mapHeight;
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
        Heightmap(unsigned int detail = 7, float tileWidth = 1.f);
        ~Heightmap();
        bool loadFromImage(const char *path, float width, float scale = 1.f);
        bool loadFromMemory(const unsigned char *buffer, int length, float width, float scale = 1.f);
        void upload();
        void render(glm::mat4x4 view, glm::vec3 center, float renderDistance);
};
