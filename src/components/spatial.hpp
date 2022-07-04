#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Spatial {
    glm::mat4 transform = glm::mat4(1);

    void translate(glm::vec3 offset) {
        transform = glm::translate(glm::mat4(1), offset) * transform;
    }

    void rotate_x(float angle) {
        transform = glm::rotate(transform, angle, glm::vec3(1, 0, 1));
    }

    void rotate_y(float angle) {
        transform = glm::rotate(transform, angle, glm::vec3(0, 1, 0));
    }

    void rotate_z(float angle) {
        transform = glm::rotate(transform, angle, glm::vec3(0, 0, 1));
    }

    glm::vec3 get_offset() {
        return glm::vec3(transform[3]);
    }
};
