#pragma once
#include <glm/ext/matrix_clip_space.hpp>
#include "spatial.hpp"

struct Camera : public Spatial {
    float fov = 45.f;
    float near = 0.01f;
    float far = 4000.f;

    glm::mat4 get_projection(float width, float height) {
        return glm::perspective(glm::radians(fov), width / height, near, far);
    }

    glm::mat4 get_view(float width, float height) {
        return get_projection(width, height) * glm::inverse(transform);
    }
};
