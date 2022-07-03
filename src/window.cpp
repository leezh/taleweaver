#include "window.hpp"
#include "gl.hpp"

GameWindow::GameWindow() {
    window = nullptr;
    context = nullptr;
    loaded = false;

    Uint32 initFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
    if (SDL_Init(initFlags)) {
        SDL_LogError(0, "%s", SDL_GetError());
        return;
    }

    int pos = SDL_WINDOWPOS_UNDEFINED;
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
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

GameWindow::operator bool() {
    return loaded;
}

GameWindow::operator SDL_Window*() {
    return window;
}
