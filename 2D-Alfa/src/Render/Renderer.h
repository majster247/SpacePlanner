#pragma once
#include <SDL2/SDL.h>
#include "../World/World.h"
#include "TextureAtlas.h"

class Renderer {
public:
    void init(SDL_Renderer* r);
    void renderWorld(World& world);

private:
    SDL_Renderer* renderer = nullptr;
    TextureAtlas atlas;
};
