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
        int get_width();
        int get_height();
        glm::vec3 &at(int x, int y);
        void resize(int new_width, int new_height);
        bool load_image_file(const char *path, float min, float max);
        bool load_image_buffer(const unsigned char *buffer, int length, float min, float max);
        void upload();

        friend class HeightmapSystem;
};

class HeightmapSystem {
    private:
        GLuint program;
        GLint loc_position;
        GLint loc_tex;
        GLint loc_xform;
        GLint loc_scale;
        GLint loc_tex_offset;
        GLint loc_tex_Scale;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        unsigned int chunk_length;
        unsigned int chunk_size;
        unsigned int stitch_size;
        unsigned int stitch_offset;

    public:
        HeightmapSystem(unsigned int detail = 7);
        ~HeightmapSystem();
        void render(const Heightmap& heightmap, CameraSystem &viewport);
};
