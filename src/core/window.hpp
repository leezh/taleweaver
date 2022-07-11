#pragma once
#include <list>
#include <functional>
#include <SDL.h>

class GameWindow {
    private:
        SDL_Window *window = nullptr;
        SDL_GLContext context = nullptr;
        bool loaded = false;
        bool running = false;

    public:
        std::list<std::function<bool(const SDL_Event &)>> on_event;
        std::list<std::function<void(float)>> on_update;
        std::list<std::function<void(int, int)>> on_render;

        GameWindow();
        ~GameWindow();
        void run();
        void stop();
};
