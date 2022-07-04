#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <entt/entt.hpp>

struct Camera {
    float fov = 45.f;
    float near = 0.01f;
    float far = 4000.f;
};

class CameraSystem {
    private:
        entt::registry &registry;
        entt::entity active_camera;
        float width;
        float height;

    public:
        CameraSystem(entt::registry &registry);
        void set_active(entt::entity entity);
        void set_viewport(float width, float height);
        entt::entity get_active();
        Camera get_camera();
        glm::vec3 get_position();
        glm::mat4 get_projection();
        glm::mat4 get_view();
};
