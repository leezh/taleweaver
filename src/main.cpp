#define GLM_FORCE_SWIZZLE
#include <iostream>
#include <stb_image.h>
#include <entt/entt.hpp>
#include "core/window.hpp"
#include "components/heightmap.hpp"
#include "components/spatial.hpp"
#include "components/camera.hpp"

const stbi_uc heightmap_data[] = {
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
    auto camera_system = CameraSystem(registry);
    auto heightmap_system = HeightmapSystem(registry);

    {
        auto entity = registry.create();
        registry.emplace<Spatial>(entity);
        registry.emplace<Camera>(entity);
        camera_system.set_active(entity);
    }

    {
        auto map = Heightmap();
        const auto sea_top = glm::u8vec4(16, 192, 224, 255);
        const auto sea_bottom = glm::u8vec4(0, 0, 128, 255);
        const auto sand = glm::u8vec4(255, 255, 192, 255);
        const auto grass_bottom = glm::u8vec4(32, 128, 64, 255);
        const auto grass_top = glm::u8vec4(192, 255, 192, 255);
        const auto snow = glm::u8vec4(255, 255, 255, 255);

        int cols, rows, n;
        auto *image = stbi_load_from_memory(heightmap_data, sizeof(heightmap_data), &cols, &rows, &n, 1);
        if (image != nullptr) {
            map.resize(cols, rows);
            auto pixel = &image[0];
            for (int y = 0; y < map.get_rows(); y++) {
                for (int x = 0; x < map.get_cols(); x++) {
                    auto &height = map.height(x, y);
                    auto &color = map.color(x, y);
                    height = (float)(*pixel++) - 100.f;
                    if (height > 80.f) {
                        color = snow;
                    } else if (height > 2.f) {
                        color = glm::mix(grass_bottom, grass_top, height / 80.f);
                    } else if (height > 0.f) {
                        color = sand;
                    } else {
                        color = glm::mix(sea_top, sea_bottom, -height / 100.f);
                    }
                }
            }
            stbi_image_free(image);
        }

        auto entity = registry.create();
        registry.emplace<Spatial>(entity);
        registry.emplace<Heightmap>(entity, std::move(map));
    }

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

        auto spatial = registry.get<Spatial>(camera_system.get_active());
        input = glm::mat3(spatial.transform) * input;
        spatial.translate(input * delta * 50.f);
        spatial.rotate_y(glm::radians(turn * delta * 50.f));
        registry.replace<Spatial>(camera_system.get_active(), spatial);
    });

    gameWindow.on_render.emplace_back([&](int width, int height) {
        camera_system.render(width, height);
    });

    camera_system.on_render.emplace_back([&](auto &view) {
        heightmap_system.render(view);
    });

    gameWindow.run();
    return 0;
}
