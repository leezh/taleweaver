#pragma once
#include <vector>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include "core/gl.hpp"
#include "core/window.hpp"
#include "components/camera.hpp"

class Heightmap {
    private:
        std::vector<glm::vec3> data;
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
        GLuint program;
        GLint locPosition;
        GLint locTex;
        GLint locXForm;
        GLint locScale;
        GLint locTexOffset;
        GLint locTexScale;

        unsigned int detail;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        unsigned int indexCount;
        unsigned int stitchIndexCount;
        void *stitchIndexOffset;

    public:
        HeightmapSystem(unsigned int detail = 7);
        ~HeightmapSystem();
        void render(const Heightmap& heightmap, CameraSystem &viewport);
};
