#define GLM_FORCE_SWIZZLE
#include <iostream>
#include <stb_image.h>
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

    Heightmap map;
    {
        const auto sea_bottom = glm::vec4(.1f, .7f, .8f, 1.f);
        const auto sea_top = glm::vec4(.0f, .0f, .5f, 1.f);
        const auto sand = glm::vec4(1.f, 1.f, .7f, 1.f);
        const auto grass_bottom = glm::vec4(.3f, .5f, .2f, 1.f);
        const auto grass_top = glm::vec4(.8f, 1.f, .8f, 1.f);
        const auto snow = glm::vec4(1.f, 1.f, 1.f, 1.f);
        int cols, rows, n;
        stbi_uc *image = nullptr;
        image = stbi_load_from_memory(heightmap_data, sizeof(heightmap_data), &cols, &rows, &n, 1);
        if (image != nullptr) {
            map.resize(cols, rows);
            auto pixel = &image[0];
            for (int y = 0; y < rows; y++) {
                for (int x = 0; x < cols; x++) {
                    float height = (float)(*pixel++) - 100.f;
                    glm::vec4 color;
                    if (height > 80.f) {
                        color = snow;
                    } else if (height > 2.f) {
                        color = glm::mix(grass_bottom, grass_top, height / 80.f);
                    } else if (height > 0.f) {
                        color = sand;
                    } else {
                        color = glm::mix(sea_bottom, sea_top, -height / 100.f);
                    }
                    map.set_height(x, y, height);
                    map.set_color(x, y, color);
                }
            }
            stbi_image_free(image);
        }
        map.generate_textures();
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
        map.render(camera);
    });

    gameWindow.run();
    return 0;
}
