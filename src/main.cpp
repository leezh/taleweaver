#define GLM_FORCE_SWIZZLE
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "window.hpp"
#include "render.hpp"
#include "heightmap.hpp"

static const unsigned char heightmap[] = {
#include "images/heightmap.png.hex"
};

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
};

struct Camera {
    float fov;
    float near;
    float far;

    glm::mat4 get_projection(float aspect) {
        return glm::perspective(glm::radians(fov), aspect, near, far);
    }
};

int main(int argc, const char *argv[]) {
    auto gameWindow = GameWindow();
    bool running = true;

    auto spatial = Spatial();
    spatial.translate(glm::vec3(0.f, 25.f, 0.f));

    auto camera = Camera{45.f, 0.01f, 4000.f};

    auto map = Heightmap(6);

    map.loadFromMemory(heightmap, sizeof(heightmap), 2000.f, 100.f);
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

        input = glm::mat3(spatial.transform) * input;
        spatial.translate(input);
        spatial.rotate_y(glm::radians(turn * delta));

        int width, height;
        SDL_GL_GetDrawableSize(gameWindow, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto view = camera.get_projection((float)width / height) * glm::inverse(spatial.transform);

        map.render(view, glm::vec3(spatial.transform[3]), 4000.f);

        SDL_GL_SwapWindow(gameWindow);
    }

    return 0;
}
