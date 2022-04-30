#define GLM_FORCE_SWIZZLE
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <stb_image.h>
#include "window.hpp"
#include "render.hpp"
#include "heightmap.hpp"

int main(int argc, const char *argv[]) {
    bool running = true;
    auto gameWindow = GameWindow();
    auto map = Heightmap(6);

    map.loadFromImage("images/heightmap.png", 2000.f, 100.f);
    map.upload();

    Uint64 elapsedTicks = SDL_GetTicks64();
    auto view = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 25.f, 0.f));
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            running = false;
                            break;
                        default:
                            break;
                    }
                default:
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
        view = glm::rotate(view, glm::radians(turn * delta), glm::vec3(0.f, 1.f, 0.f));
        view = glm::translate(view, input * 20.f * delta);

        int width, height;
        SDL_GL_GetDrawableSize(gameWindow, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto perspective = glm::perspective(glm::radians(45.f), (float)width / height, 0.01f, 4000.f);
        auto xform = perspective * glm::inverse(view);

        map.render(xform, glm::vec3(view[3]), 1000.f);

        SDL_GL_SwapWindow(gameWindow);
    }

    return 0;
}
