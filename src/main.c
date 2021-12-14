#include <stdio.h>
#include <glad/gl.h>
#include "main.h"

SDL_Window *window = NULL;
SDL_GLContext context = NULL;
int running = 1;

int main(int argc, const char *argv[]) {
    Uint32 initFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
    if (SDL_Init(initFlags)) {
        SDL_LogError(0, "%s", SDL_GetError());
        return -1;
    }

    int pos = SDL_WINDOWPOS_UNDEFINED;
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("Taleweaver", pos, pos, 1280, 720, windowFlags);
    if (!window) {
        SDL_LogError(0, "%s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_LogError(0, "%s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    Uint64 elapsedTicks = SDL_GetTicks64();
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE:
                            running = 0;
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

        int width, height;
        SDL_GL_GetDrawableSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();
    return 0;
}
