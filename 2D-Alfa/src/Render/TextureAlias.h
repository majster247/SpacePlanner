#pragma once
#include <SDL2/SDL.h>
#include "../World/Chunk.h"

class TextureAtlas {
public:
    bool load(SDL_Renderer* r, const char* filename);

    void drawTile(SDL_Renderer* r, TileType type, int screenX, int screenY);

private:
    SDL_Texture* atlas = nullptr;
    int tileSize = 16;
};
