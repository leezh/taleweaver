#include "window.hpp"
#include "render.hpp"
#include "heightmap.hpp"

int main(int argc, const char *argv[]) {
    bool running = true;
    GameWindow gameWindow;
    if (!gameWindow) return -1;

    renderInit();
    heightmapInit();

    Uint64 elapsedTicks = SDL_GetTicks64();
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

        int width, height;
        SDL_GL_GetDrawableSize(gameWindow, &width, &height);
        renderPrepare(width, height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        heightmapRender();

        SDL_GL_SwapWindow(gameWindow);
    }

    heightmapQuit();
    renderQuit();
    return 0;
}
