#include "App.h"

App::App(int w, int h)
: width(w), height(h), world(12345) {}

bool App::init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SpacePlanner 2D",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN);

    rendererSDL = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    renderer.init(rendererSDL);
    world.init();

    return true;
}

void App::run() {
    bool running = true;
    SDL_Event e;

    while(running) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = false;
        }

        // generuj brakujące chunky
        world.update();

        // renderowanie
        SDL_SetRenderDrawColor(rendererSDL, 25, 25, 25, 255);
        SDL_RenderClear(rendererSDL);

        renderer.renderWorld(world);

        SDL_RenderPresent(rendererSDL);
    }
}

void App::shutdown() {
    SDL_DestroyRenderer(rendererSDL);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
