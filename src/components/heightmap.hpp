#pragma once
#include <vector>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include "core/gl.hpp"
#include "core/window.hpp"

class Heightmap {
    private:
        std::vector<float> data;
        int width;
        int height;
        GLuint texture = 0;

    public:
        ~Heightmap();
        bool loadFromImage(const char *path, float min, float max);
        bool loadFromMemory(const unsigned char *buffer, int length, float min, float max);
        void upload();

        friend class HeightmapSystem;
};

class HeightmapSystem {
    private:
        unsigned int detail;
        GLuint program;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLint locPosition;
        GLint locTex;
        GLint locXForm;
        GLint locScale;
        GLint locTexOffset;
        GLint locTexScale;
        unsigned int indexCount;
        unsigned int borderIndexCount;
        void *borderOffset;

    public:
        HeightmapSystem(unsigned int detail = 7);
        ~HeightmapSystem();
        void render(const Heightmap& heightmap, glm::mat4x4 view, glm::vec3 center, float renderDistance);
};
