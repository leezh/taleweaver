#pragma once
#include <array>
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "core/gl.hpp"
#include "core/window.hpp"
#include "components/camera.hpp"

class Heightmap {
    private:
        std::vector<float> height_data;
        std::vector<std::array<std::uint8_t, 2>> normal_data;
        std::vector<std::array<std::uint8_t, 4>> color_data;
        int cols;
        int rows;

        unsigned int detail = 7;
        bool init_gl = false;
        bool init_textures = false;

        GLuint height_texture;
        GLuint normal_texture;
        GLuint color_texture;
        GLuint program;
        GLint loc_position;
        GLint loc_height_texture;
        GLint loc_xform;
        GLint loc_scale;
        GLint loc_tex_offset;
        GLint loc_tex_Scale;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint chunk_size;
        GLuint stitch_size;
        GLuint stitch_offset;

    public:
        ~Heightmap();
        int get_cols();
        int get_rows();
        float &height(int x, int y);
        void resize(int new_cols, int new_rows);
        void generate_normals();
        void generate_textures();
        void render(CameraSystem &viewport);
};
