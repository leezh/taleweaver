#pragma once
#include <SDL.h>
#include <glm/glm.hpp>

class GameWindow {
    private:
        SDL_Window *window;
        SDL_GLContext context;
        bool loaded;

    public:
        GameWindow();
        ~GameWindow();
        operator bool();
        operator SDL_Window*();
};
