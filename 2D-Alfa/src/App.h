#pragma once
#include <SDL2/SDL.h>
#include "World/World.h"
#include "Render/Renderer.h"

class App {
public:
    App(int w, int h);
    bool init();
    void run();
    void shutdown();

private:
    int width, height;
    SDL_Window* window = nullptr;
    SDL_Renderer* rendererSDL = nullptr;

    World world;
    Renderer renderer;
};
