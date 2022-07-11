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
    map.loadFromImageBuffer(heightmap_data, sizeof(heightmap_data), -100.f, 100.f);

    for (int x = 0; x < map.get_width(); x++) {
        map.at(x, 0) = glm::vec3(0);
        map.at(x, map.get_height() - 1) = glm::vec3(0);
    }
    for (int y = 0; y < map.get_height(); y++) {
        map.at(0, y) = glm::vec3(0);
        map.at(map.get_width() - 1, y) = glm::vec3(0);
    }
    map.upload();

    bool dragEnable;
    int dragStartX;
    int dragStartY;
    glm::vec3 dragInput;

    gameWindow.onEvent.emplace_back([&](const SDL_Event &event) {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            gameWindow.stop();
            return true;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 1) {
            dragEnable = true;
            dragStartX = event.button.x;
            dragStartY = event.button.y;
        }
        if (event.type == SDL_MOUSEBUTTONUP && event.button.button == 1) {
            dragEnable = false;
            dragInput = glm::vec3(0);
        }
        if (event.type == SDL_MOUSEMOTION && dragEnable) {
            dragInput.x = event.motion.x - dragStartX;
            dragInput.z = event.motion.y - dragStartY;
        }
        return false;
    });

    gameWindow.onUpdate.emplace_back([&](float delta) {
        auto input = glm::vec3(0.f, 0.f, 0.f) + dragInput;
        auto turn = 0.f;
        const Uint8 *state = SDL_GetKeyboardState(nullptr);
        if (state[SDL_GetScancodeFromKey(SDLK_w)]) input.z -= 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_s)]) input.z += 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_a)]) turn += 50.f;
        if (state[SDL_GetScancodeFromKey(SDLK_d)]) turn -= 50.f;
        if (state[SDL_GetScancodeFromKey(SDLK_q)]) input.y -= 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_e)]) input.y += 1.f;
        if (glm::length(input) > 1.f) input = glm::normalize(input);

        auto spatial = registry.get<Spatial>(camera.get_active());
        input = glm::mat3(spatial.transform) * input;
        spatial.translate(input);
        spatial.rotate_y(glm::radians(turn * delta));
        registry.replace<Spatial>(camera.get_active(), spatial);
    });

    gameWindow.onRender.emplace_back([&](int width, int height) {
        camera.render(width, height);
    });

    camera.onRender.emplace_back([&](int width, int height) {
        mapRender.render(map, camera);
    });

    gameWindow.run();
    return 0;
}
