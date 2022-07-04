#include "window.hpp"
#include "core/gl.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GameWindow::GameWindow() {
    Uint32 initFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
    if (SDL_Init(initFlags)) {
        SDL_LogError(0, "%s", SDL_GetError());
        return;
    }

    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

    int pos = SDL_WINDOWPOS_UNDEFINED;
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP;
    window = SDL_CreateWindow("Taleweaver", pos, pos, 1280, 720, windowFlags);
    if (!window) {
        SDL_LogError(0, "%s", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_LogError(0, "%s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

#ifndef USE_GLES
    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
#else
    gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress);
#endif

    loaded = true;
}

GameWindow::~GameWindow() {
    if (loaded) {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}

void GameWindow::run() {
    running = true;
    Uint64 elapsedTicks = SDL_GetTicks64();
    while (loaded && running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
                running = false;
                break;
            }

            for (auto callback : onEvent) {
                if (callback(event)) break;
            }
        }

        Uint64 previousTicks = elapsedTicks;
        elapsedTicks = SDL_GetTicks64();
        float delta = (float)(elapsedTicks - previousTicks) / 1000.f;

        for (auto callback : onUpdate) {
            callback(delta);
        }

        int width, height;
        SDL_GL_GetDrawableSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto callback : onRender) {
            callback(width, height);
        }

        SDL_GL_SwapWindow(window);
    }
}

void GameWindow::stop() {
    running = false;
}