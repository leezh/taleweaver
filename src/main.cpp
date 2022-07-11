#define GLM_FORCE_SWIZZLE
#include <iostream>
#include <entt/entt.hpp>
#include "core/window.hpp"
#include "components/heightmap.hpp"
#include "components/spatial.hpp"
#include "components/camera.hpp"

static const unsigned char heightmap_data[] = {
#include "images/heightmap.png.hex"
};


#ifndef USE_JNI
int main(int argc, const char *argv[]) {
#else
#include <jni.h>
extern "C" JNIEXPORT jint JNICALL
Java_net_leezh_taleweaver_MainActivity_nativeRunMain(JNIEnv* env, jclass clazz) {
#endif
    auto gameWindow = GameWindow();
    auto registry = entt::registry();

    auto camera = CameraSystem(registry);
    camera.set_active(registry.create());

    HeightmapSystem mapRender;
    Heightmap map;
    map.load_image_buffer(heightmap_data, sizeof(heightmap_data), -100.f, 100.f);

    for (int x = 0; x < map.get_width(); x++) {
        map.at(x, 0) = glm::vec3(0);
        map.at(x, map.get_height() - 1) = glm::vec3(0);
    }
    for (int y = 0; y < map.get_height(); y++) {
        map.at(0, y) = glm::vec3(0);
        map.at(map.get_width() - 1, y) = glm::vec3(0);
    }
    map.upload();

    bool drag_enable = false;
    glm::vec2 drag_start;
    glm::vec2 drag_input;

    gameWindow.on_event.emplace_back([&](const SDL_Event &event) {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            gameWindow.stop();
            return true;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 1) {
            drag_enable = true;
            drag_start = glm::vec2(event.button.x, event.button.y);
            drag_input = glm::vec2(0, 0);
        }
        if (event.type == SDL_MOUSEBUTTONUP && event.button.button == 1) {
            drag_enable = false;
        }
        if (event.type == SDL_MOUSEMOTION && drag_enable) {
            drag_input = glm::vec2(event.motion.x, event.motion.y) - drag_start;
        }
        return false;
    });

    gameWindow.on_update.emplace_back([&](float delta) {
        auto input = glm::vec3(0.f, 0.f, 0.f);
        auto turn = 0.f;
        const Uint8 *state = SDL_GetKeyboardState(nullptr);
        if (drag_enable) input.z += glm::round(drag_input.y / 100.f);
        if (drag_enable) turn -= glm::round(drag_input.x / 100.f);
        if (state[SDL_GetScancodeFromKey(SDLK_w)]) input.z -= 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_s)]) input.z += 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_a)]) turn += 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_d)]) turn -= 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_q)]) input.y -= 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_e)]) input.y += 1.f;
        if (glm::length(input) > 1.f) input = glm::normalize(input);
        turn = glm::clamp(turn, -1.f, 1.f);

        auto spatial = registry.get<Spatial>(camera.get_active());
        input = glm::mat3(spatial.transform) * input;
        spatial.translate(input * delta * 50.f);
        spatial.rotate_y(glm::radians(turn * delta * 50.f));
        registry.replace<Spatial>(camera.get_active(), spatial);
    });

    gameWindow.on_render.emplace_back([&](int width, int height) {
        camera.render(width, height);
    });

    camera.on_render.emplace_back([&](int width, int height) {
        mapRender.render(map, camera);
    });

    gameWindow.run();
    return 0;
}
