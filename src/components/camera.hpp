#pragma once
#include <functional>
#include <list>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <entt/entt.hpp>
#include "core/gl.hpp"

struct Camera {
    float fov = 45.f;
    float near = 0.01f;
    float far = 4000.f;

    Camera() = default;
    Camera(Camera &&camera) = default;
    Camera(const Camera &camera) = default;
    Camera& operator=(Camera &&camera) = default;
    Camera& operator=(const Camera &camera) = default;
};

class CameraSystem {
    private:
        entt::registry &registry;
        entt::entity active_camera;
        int width = 0;
        int height = 0;
        GLuint depth_texture = 0;
        GLuint color_texture = 0;
        GLuint program = 0;
        GLuint fbo = 0;
        GLuint vbo = 0;
        GLuint vao = 0;
        GLint loc_position;
        GLint loc_tex;

    public:
        float render_scale = 1.f;
        std::list<std::function<void(const glm::mat4 &)>> on_render;

        CameraSystem(entt::registry &registry);
        ~CameraSystem();
        void set_active(entt::entity entity);
        void set_viewport(int width, int height);
        entt::entity get_active();
        Camera get_camera();
        glm::vec3 get_position();
        glm::mat4 get_projection();
        glm::mat4 get_view();
        void render(int screen_width, int screen_height);
};
