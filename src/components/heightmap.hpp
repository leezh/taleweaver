#pragma once
#include <array>
#include <vector>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_int4.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include "core/gl.hpp"
#include "core/window.hpp"
#include "components/spatial.hpp"
#include "components/camera.hpp"

class HeightmapCache {
    private:
        GLuint height_texture = 0;
        GLuint normal_texture = 0;
        GLuint color_texture = 0;
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLuint elements = 0;

    public:
        HeightmapCache() = default;
        ~HeightmapCache();
        HeightmapCache(HeightmapCache &&) = default;
        HeightmapCache& operator=(HeightmapCache &&) = default;

        friend class Heightmap;
        friend class HeightmapSystem;
};

class Heightmap {
    private:
        std::vector<float> height_data;
        std::vector<glm::i8vec2> normal_data;
        std::vector<glm::u8vec4> color_data;
        int cols = 0;
        int rows = 0;

    public:
        Heightmap() = default;
        ~Heightmap() = default;
        Heightmap(Heightmap &&) = default;
        Heightmap(const Heightmap &) = default;
        Heightmap& operator=(Heightmap &&) = default;
        Heightmap& operator=(const Heightmap &) = default;

        void resize(int new_cols, int new_rows);
        int get_cols() const;
        int get_rows() const;
        float &height(int x, int y);
        const float &height(int x, int y) const;
        glm::u8vec4 &color(int x, int y);
        const glm::u8vec4 &color(int x, int y) const;

        void generate_normals();

        friend class HeightmapSystem;
};

class HeightmapSystem {
    private:
        entt::registry &registry;
        bool init_gl = false;
        GLuint program;
        GLint loc_position;
        GLint loc_height_texture;
        GLint loc_normal_texture;
        GLint loc_color_texture;
        GLint loc_xform;

    public:
        HeightmapSystem(entt::registry &registry);
        ~HeightmapSystem();
        void render(const glm::mat4 &view);
};
