#define GLM_FORCE_SWIZZLE
#include <iostream>
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

    gameWindow.onEvent.emplace_back([&](const SDL_Event &event){
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            gameWindow.stop();
            return true;
        }
        return false;
    });

    Camera camera;
    gameWindow.onUpdate.emplace_back([&](float delta){
        auto input = glm::vec3(0.f, 0.f, 0.f);
        auto turn = 0.f;
        const Uint8 *state = SDL_GetKeyboardState(nullptr);
        if (state[SDL_GetScancodeFromKey(SDLK_w)]) input.z -= 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_s)]) input.z += 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_a)]) turn += 50.f;
        if (state[SDL_GetScancodeFromKey(SDLK_d)]) turn -= 50.f;
        if (state[SDL_GetScancodeFromKey(SDLK_q)]) input.y -= 1.f;
        if (state[SDL_GetScancodeFromKey(SDLK_e)]) input.y += 1.f;
        if (glm::length(input) > 1.f) input = glm::normalize(input);

        input = glm::mat3(camera.transform) * input;
        camera.translate(input);
        camera.rotate_y(glm::radians(turn * delta));
    });

    HeightmapSystem mapRender;
    Heightmap map;
    map.loadFromMemory(heightmap_data, sizeof(heightmap_data), -100.f, 100.f);

    gameWindow.onRender.emplace_back([&](int width, int height){
        auto view = camera.get_view((float)width, (float)height);
        mapRender.render(map, view, camera.get_offset(), 4000.f);
    });

    gameWindow.run();
    return 0;
}
