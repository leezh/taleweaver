#define GLM_FORCE_SWIZZLE
#include <iostream>
#include <glm/trigonometric.hpp>
#include "window.hpp"
#include "render.hpp"
#include "spatial.hpp"
#include "camera.hpp"
#include "heightmap.hpp"

static const unsigned char heightmap[] = {
#include "images/heightmap.png.hex"
};

int main(int argc, const char *argv[]) {
    auto gameWindow = GameWindow();
    bool running = true;

    auto camera = Camera();
    auto map = Heightmap(7);

    map.loadFromMemory(heightmap, sizeof(heightmap), -100.f, 100.f);
    map.upload();

    Uint64 elapsedTicks = SDL_GetTicks64();
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
                break;
            }
        }

        Uint64 previousTicks = elapsedTicks;
        elapsedTicks = SDL_GetTicks64();
        float delta = (float)(elapsedTicks - previousTicks) / 1000.f;

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

        int width, height;
        SDL_GL_GetDrawableSize(gameWindow, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto view = camera.get_view(width, height);

        map.render(view, camera.get_offset(), 4000.f);

        SDL_GL_SwapWindow(gameWindow);
    }

    return 0;
}

#ifdef USE_JNI

#include <jni.h>
extern "C" JNIEXPORT jint JNICALL
Java_net_leezh_taleweaver_MainActivity_nativeRunMain(JNIEnv* env, jclass clazz) {
    const char* name = "taleweaver";
    return main(0, &name);
}

#endif
