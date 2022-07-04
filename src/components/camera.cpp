#include "camera.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "components/spatial.hpp"

CameraSystem::CameraSystem(entt::registry &registry): registry(registry) {
    registry.on_construct<Camera>().connect<&entt::registry::emplace_or_replace<Spatial>>();
}

void CameraSystem::set_active(entt::entity entity) {
    registry.emplace_or_replace<Camera>(entity);
    active_camera = entity;
}

void CameraSystem::set_viewport(float new_width, float new_height) {
    width = new_width;
    height = new_height;
}

entt::entity CameraSystem::get_active() {
    return active_camera;
}

Camera CameraSystem::get_camera() {
    return registry.get<Camera>(active_camera);
}

glm::vec3 CameraSystem::get_position() {
    auto spatial = registry.get<Spatial>(active_camera);
    return glm::vec3(spatial.transform[3]);
}

glm::mat4 CameraSystem::get_projection() {
    auto camera = registry.get<Camera>(active_camera);
    return glm::perspective(glm::radians(camera.fov), width / height, camera.near, camera.far);
}

glm::mat4 CameraSystem::get_view() {
    auto spatial = registry.get<Spatial>(active_camera);
    return get_projection() * glm::inverse(spatial.transform);
}
